#include <dav/optional.hpp>
#include <kernel/FreeListAllocator.h>
#include <kernel/kernel.h>
#include <kernel/macros.h>
#include <kernel/paging.h>
#include <kernel/vmm.h>

using allocated_type = std::byte;
using virtual_allocator_type = FreeListAllocator<allocated_type>;

static auto allocator = virtual_allocator_type {};

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
    allocator.add_memory_impl(reinterpret_cast<allocated_type *>(base), size);
}

auto vmalloc(size_t size) -> void * {
    return allocator.allocate_impl(size);
}

auto vfree(void *ptr) -> void {
    allocator.deallocate_impl(reinterpret_cast<allocated_type *>(ptr), 0);
}