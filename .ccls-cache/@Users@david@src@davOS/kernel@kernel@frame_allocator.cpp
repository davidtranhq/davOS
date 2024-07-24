/**
 * @file frame_allocator.cpp
 * @brief A frame allocator implemented using a free stack.
 * 
 * The allocator maintains a stack containing the addresses of available frames.
 * Allocating a new frame pops from the stack.
 * Deallocating a frame pushes the address of the frame to the stack.
 * Initially, the stack contains the addresses of all available after booting.
 */

#include <stddef.h>
#include <stdio.h>

#include <dav/algorithm.hpp>
#include <dav/optional.hpp>
#include <dav/allocated_stack.hpp>
#include <dav/allocated_vector.hpp>
#include <kernel/constants.h>
#include <kernel/frame_allocator.h>
#include <kernel/kernel.h>
#include <kernel/limine_features.h>
#include <kernel/macros.h>

static auto free_stack = dav::optional<dav::allocated_stack> {};

struct FrameRange {
    uintptr_t begin;
    uintptr_t end;
};

/**
 * @brief Check if an entry in the Limine memory map is allocatable (available for use)
 */
static bool is_allocatable(struct limine_memmap_entry *entry)
{
    return entry->type == LIMINE_MEMMAP_USABLE;
}

/**
 * @brief Calculate the total number of allocatable frames for use in this system.
 */
static size_t get_num_allocatable_frames()
{
    size_t allocatable_frames = 0;
    for (size_t i = 0; i < limine::memory_map->entry_count; ++i)
    {
        struct limine_memmap_entry *entry = limine::memory_map->entries[i];
        if (is_allocatable(entry))
            allocatable_frames += entry->length / frame_size;
    }
    return allocatable_frames;
}

/**
 * @brief Return the ceiling of a division.
 */
static constexpr auto ceil_div(auto dividend, auto divisor)
{
    return (dividend / divisor) + (dividend % divisor != 0);
}

/**
 * @brief Reserves contiguous frames from the system.
 * 
 * Since the free stack is not initailized yet when this function is called, we can't use
 * the free stack to allocate memory; instead, we reserve
 * the frames by manually examining the memory map provided by Limine.
 * 
 * This function should NOT be used to allocate memory for anything else
 * other than data structures required before the physical memory manager is initialized.
 * 
 * @returns A pointer the start (low address) of the contiguous frames
 */
static uintptr_t manually_reserve_contiguous_frames(size_t num_frames)
{
    // look for a contiguous memory segment that is big enough to hold num_frames,
    // and is allocatable
    for (size_t i = 0; i < limine::memory_map->entry_count; ++i)
    {
        struct limine_memmap_entry *entry = limine::memory_map->entries[i];
        // check if this segment is allocatable and big enough
        if (is_allocatable(entry) && entry->length >= num_frames * frame_size)
            return entry->base;
    }
    // no viable segment was found
    return 0;
}

/**
 * @brief Initialize the free stack with all the allocatable frames, excluding the given ranges.
 * 
 * @param exclude_ranges A list of ranges to exclude from the free stack.
 */
template <size_t num_exclude_ranges>
static void fill_free_stack(dav::Array<FrameRange, num_exclude_ranges> const &exclude_ranges)
{
    auto const in_exclude_range = [&exclude_ranges](uintptr_t frame) {
        for (auto const &exclude_range : exclude_ranges) {
            if (frame >= exclude_range.begin && frame < exclude_range.end)
                return true;
        }
        return false;
    };

    // add the allocatable frames from each segment to the free stack
    for (size_t i = 0; i < limine::memory_map->entry_count; ++i)
    {
        struct limine_memmap_entry *entry = limine::memory_map->entries[i];
        if (!is_allocatable(entry))
            continue;

        // push the allocatable frames from this segment to the free stack
        uint64_t end_of_entry = entry->base + entry->length;
        for (uintptr_t frame = entry->base; frame < end_of_entry; frame += frame_size)
        {
            // do not include frames that were allocated for the free stack
            if (in_exclude_range(frame))
                continue;
            free_stack->push(reinterpret_cast<void *>(frame));
        }
    }
}

void frame_allocator_init()
{
    DEBUG("Initializing frame allocator...\n");
    // count how many frames of memory we can allocate to the user in total
    // (and hence the max size of the free stack)
    size_t num_allocatable_frames = get_num_allocatable_frames();
    DEBUG("Found %d allocatable frames\n", num_allocatable_frames);

    // the free stack needs enough frames for itself to be able to store (pointers)
    // to all allocatable frames
    // one free stack frame can hold (frame_size_in_bytes / 8) frame pointers per frame
    // (assuming 64-bit = 8-byte pointers)
    size_t frame_pointers_per_frame = ceil_div(frame_size, 8);
    size_t num_free_stack_frames = ceil_div(num_allocatable_frames, frame_pointers_per_frame);

    // get contiguous frames for the free stack
    uintptr_t free_stack_frames_begin = manually_reserve_contiguous_frames(num_free_stack_frames);
    DEBUG("Reserved %d contiguous frames for the free stack\n", num_free_stack_frames);

    if (!free_stack_frames_begin)
        kernel_panic("not enough contiguous space for the free stack frame allocator");

    // one-past-the-end of the contiguous stack frames
    uintptr_t free_stack_frames_end = free_stack_frames_begin + num_free_stack_frames * frame_size;

    // construct the free stack with the allocated memory
    free_stack.emplace(reinterpret_cast<void *>(free_stack_frames_begin), 
                       num_allocatable_frames);

    // fill the free stack with the allocatable frames
    auto const exclude_ranges = dav::Array<FrameRange, 1> {
        FrameRange {free_stack_frames_begin, free_stack_frames_end}
    };

    fill_free_stack(exclude_ranges);
    DEBUG("Finished initializing the frame allocator with %d free frames\n", free_stack->size());
}

void *allocate_frame()
{
    if (free_stack->empty())
        kernel_panic("ran out of physical memory to allocate!");
    auto frame_address = free_stack->top();
    free_stack->pop();
    return frame_address;
}

uint64_t available_frames()
{
    return free_stack->size();
}

void deallocate_frame(void *frame)
{
    free_stack->push(frame);
}

void *kernel_physical_to_virtual(void *physical_address)
{
    return reinterpret_cast<void *>(
        kernel_physical_to_virtual(reinterpret_cast<uintptr_t>(physical_address)));
}

uintptr_t kernel_physical_to_virtual(uintptr_t physical_address)
{
    return physical_address + limine::hhdm_address->offset;
}

void free_limine_bootloader_memory()
{
    uint64_t reclaimed_frames = 0;
    // add the allocatable frames from each segment to the free stack
    for (size_t i = 0; i < limine::memory_map->entry_count; ++i)
    {
        struct limine_memmap_entry *entry = limine::memory_map->entries[i];
        // only reclaim bootloader-reclaimable entries
        if (entry->type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
            continue;

        // push the allocatable frames from this segment to the free stack
        uint64_t end_of_entry = entry->base + entry->length;
        for (uintptr_t frame = entry->base; frame < end_of_entry; frame += frame_size)
        {
            reclaimed_frames += 1;
            deallocate_frame(reinterpret_cast<void *>(frame));
        }
    }

    DEBUG("Reclaimed %d frames of Limine bootloader memory, %d available frames\n",
          reclaimed_frames, available_frames());
}

const char *limine_memmap_type(int type)
{
    switch (type)
    {
    case 0: return "USABLE";
    case 1: return "RESERVED";
    case 2: return "ACPI_RECLAIMABLE";
    case 3: return "ACPI_NVS";
    case 4: return "BAD_MEMORY";
    case 5: return "BOOTLOADER_RECLAIMABLE";
    case 6: return "KERNEL_AND_MODULES";
    case 7: return "FRAMEBUFFER";
    default: return "unknown";
    }
}

void print_memory_map()
{
    for (size_t i = 0; i < limine::memory_map->entry_count; ++i)
    {
        struct limine_memmap_entry *entry = limine::memory_map->entries[i];
        printf("base: %x, limit: %x, type: %s\n", entry->base, entry->length,
            limine_memmap_type(entry->type));
    }
}

auto update_frame_ref_count(uintptr_t frame, int change) -> void
{
    // for now, just assume each frame can only be referenced once, so decrementing it
    // is equivalent to free-ing it
    if (change == -1) {
        deallocate_frame(reinterpret_cast<void *>(frame));
    }
}
