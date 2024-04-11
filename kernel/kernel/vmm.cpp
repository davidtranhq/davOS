#include <dav/optional.hpp>
#include <kernel/constants.h>
#include <kernel/frame_allocator.h>
#include <kernel/FreeListAllocator.h>
#include <kernel/kernel.h>
#include <kernel/macros.h>
#include <kernel/paging.h>
#include <kernel/vmm.h>

using allocated_type = std::byte;
using virtual_allocator_type = FreeListAllocator<allocated_type>;

static auto allocator = Allocator<virtual_allocator_type> {};

auto vmm_init() -> void {
    const auto free_virtual_memory_regions = paging_get_initial_free_regions();
    for (const auto &[base, size] : free_virtual_memory_regions) {
        if (size == 0) {
            continue;
        }
        add_virtual_memory(reinterpret_cast<void *>(base), size);
        DEBUG("Initialized VMM with region at %x with size %x\n", base, size);
    }
    DEBUG("Initialized VMM.\n");
}

auto add_virtual_memory(void *base, size_t size) -> void {
    allocator.add_memory(reinterpret_cast<allocated_type *>(base), size);
}

auto update_ref_count_of_frames_in_region(allocated_type *ptr, int change) -> void {
    auto const size = allocator.get_size(ptr);
    auto const [start, end] = get_page_range(reinterpret_cast<uintptr_t>(ptr), size);
    for (auto page = start; page != end; page += page_size) {
        update_frame_ref_count(paging_get_translation(page).physical_address, change);
    }
}

auto vmalloc(size_t size) -> void * {
    auto const ptr = allocator.allocate(size);
    update_ref_count_of_frames_in_region(ptr, 1);
    return ptr;
}

auto vfree(void *ptr) -> void {
    auto const casted_ptr = reinterpret_cast<allocated_type *>(ptr);
    allocator.deallocate(casted_ptr, 0);
    update_ref_count_of_frames_in_region(casted_ptr, -1);
}