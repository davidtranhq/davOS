#ifndef DAVOS_KERNEL_VMM_H_INCLUDED
#define DAVOS_KERNEL_VMM_H_INCLUDED

#include <kernel/Allocator.h>

auto vmm_init() -> void;

/**
 * @brief Allocate a contiguous region of virtual address space.
 */
auto vmalloc(size_t size) -> void *;

auto vfree(void *ptr) -> void;

#endif
