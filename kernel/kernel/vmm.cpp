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
    for (const auto &[base, size] : paging_get_initial_free_regions()) {
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

auto vmalloc(size_t size) -> void * {
    return allocator.allocate(size);
}

auto vfree(void *ptr) -> void {
    allocator.deallocate(reinterpret_cast<allocated_type *>(ptr));
    // assume frames cannot be referenced multiple times for now: deallocate the frame mapped to
    // by this virtual address
    deallocate_frame(
        reinterpret_cast<void *>(
            paging_get_translation(
                reinterpret_cast<uintptr_t>(
                    ptr
                )
            ).physical_address
        )
    );
}