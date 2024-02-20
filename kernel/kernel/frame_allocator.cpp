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
#include <stdint.h>
#include <stdio.h>

#include <dav/optional.hpp>
#include <kernel/constants.h>
#include <kernel/frame_allocator.h>
#include <kernel/kernel.h>
#include <kernel/limine_features.h>
#include <kernel/macros.h>

namespace
{

/**
 * @brief A stack that does not own the memory it uses to store its stack contents:
 * a fixed memory allocation needs to be passed in through the constructor and managed externally.
 * 
 * It also cannot grow; the size is fixed and determined when the stack is constructed.
 * 
 * These restrictions exist to allow it to be used in an environnment where a dynamic
 * memory allocator doesn't yet exist.
 */
class FixedStack
{
public:
    FixedStack(void *base, size_t max_size) noexcept
        : stack_ {reinterpret_cast<void **>(base)},
          max_size_ {max_size}
    {}

    void push(void *frame_pointer) noexcept
    {
        if (full())
            return;
        stack_[size_] = frame_pointer;
        size_ += 1;
    }

    void pop() noexcept
    {
        if (empty())
            return;
        size_ -= 1;
    }

    void *top() const noexcept
    {
        return stack_[size_ - 1];
    }

    bool empty() const noexcept
    {
        return size_ == 0;
    }

    bool full() const noexcept
    {
        return size_ == max_size_;
    }

    size_t size() const noexcept
    {
        return size_;
    }

private:
    void **stack_ = 0;
    size_t max_size_ = 0;
    size_t size_ = 0;
};

auto free_stack = dav::optional<FixedStack> {};

/**
 * @brief Check if an entry in the Limine memory map is allocatable (available for use)
 */
bool is_allocatable(struct limine_memmap_entry *entry)
{
    return entry->type == LIMINE_MEMMAP_USABLE;
}

/**
 * @brief Calculate the total number of allocatable frames for use in this system.
 */
size_t get_num_allocatable_frames()
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
inline constexpr auto ceil_div(auto dividend, auto divisor)
{
    return (dividend / divisor) + (dividend % divisor != 0);
}

/**
 * @brief Reserve the intiial contiguous frames required to initialize the free stack.
 * 
 * Since the free stack is not initailized yet when this function is called, we can't use
 * the free stack to allocate memory; instead, we reserve
 * the frames by manually examining the memory map provided by Limine.
 * 
 * This function should NOT be used to allocate memory for anything else
 * other than the free stack: it's only purpose is to allocate memory for the free stack itself
 * 
 * @returns A pointer the start (low address) of the contiguous frames
 */
uintptr_t allocate_initial_contiguous_frames(size_t num_frames)
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
 * @brief Initialize the free stack with all the allocatable frames.
 * We exclude the frames that were used to allocate the stack itself.
 * 
 * @param free_stack_frames_begin the start of the contiguous free stack frames
 * @param free_stack_frames_end one past the end of the contiguous free stack frames
 */
void fill_free_stack(
    uintptr_t free_stack_frames_begin,
    uintptr_t free_stack_frames_end
)
{
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
            if (frame >= free_stack_frames_begin && frame < free_stack_frames_end)
                continue;
            free_stack->push(reinterpret_cast<void *>(frame));
        }
    }
}

} // anonymous namespace

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
    uintptr_t free_stack_frames_begin = allocate_initial_contiguous_frames(num_free_stack_frames);
    DEBUG("Allocated %d contiguous frames for the free stack\n", num_free_stack_frames);

    if (!free_stack_frames_begin)
        kernel_panic("not enough contiguous space for the free stack frame allocator");

    // one-past-the-end of the contiguous stack frames
    uintptr_t free_stack_frames_end = free_stack_frames_begin + num_free_stack_frames * frame_size;

    // construct the free stack with the allocated memory
    free_stack.emplace(reinterpret_cast<void *>(free_stack_frames_begin), 
                       num_allocatable_frames);

    // fill the free stack with the allocatable frames
    // (does not include the frames we just allocated for the free stack itself)
    fill_free_stack(free_stack_frames_begin, free_stack_frames_end);
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