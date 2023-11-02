/**
 * @file vmm.h
 * @brief Interface for the virtual memory manager
 */

#ifndef DAVOS_KERNEL_VMM_H_INCLUDED
#define DAVOS_KERNEL_VMM_H_INCLUDED

#include <stdint.h>

/**
 * @brief Permission, access and cache policy flags for virtual-to-physical mappings.
 */
enum PageFlags : uint64_t
{
    None = 0,
    Write = 1ULL << 1,        // allow writes
    User = 1ULL << 2,         // allow user-mode access
    WriteThrough = 1ULL << 3, // use write-through caching policy
    CacheDisable = 1ULL << 4,
    ExecuteDisable = 1ULL << 63, // disable instruction fetches
};

/**
 * @brief Allocate space for the new page table, and re-map essential mappings
 * mapped by Limine. Then free the old page table by reclaiming bootloader reclaimable
 * memory.
 */
void vmm_init();

/**
 * @brief Add a virtual to physical mapping to the tree.
 * Physical base should point to a contiguous region in physical memory of length
 * `length` whose frames have already been allocated.
 *
 * @param virtual_base
 * @param physical_base
 * @param length
 * @param flags permission and cache policy flags
 */
void vmm_add_mapping(uintptr_t virtual_base,
                     uintptr_t physical_base,
                     uint64_t length,
                     PageFlags flags);

uintptr_t vmm_virtual_to_physical(uintptr_t virtual_address);
#endif