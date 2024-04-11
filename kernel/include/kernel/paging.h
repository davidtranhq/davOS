/**
 * @file paging.h
 * @brief Interface for the virtual memory manager
 */

#ifndef DAVOS_KERNEL_PAGING_H_INCLUDED
#define DAVOS_KERNEL_PAGING_H_INCLUDED

#include <stdint.h>
#include <dav/array.hpp>

/**
 * @brief Permission, access and cache policy flags for virtual-to-physical mappings.
 */
enum PageFlags : uint64_t
{
    None = 0,
    Present = 1ULL,
    Write = 1ULL << 1,        // allow writes
    User = 1ULL << 2,         // allow user-mode access
    WriteThrough = 1ULL << 3, // use write-through caching policy
    CacheDisable = 1ULL << 4,
    ExecuteDisable = 1ULL << 63, // disable instruction fetches
};

struct MemoryRegion {
    uintptr_t base {};
    size_t size {};
};

struct PageRange {
    uintptr_t first_page = 0;
    uintptr_t last_page = 0;
};

/**
 * @brief 
 * 
 */
struct PageTranslation {
    uintptr_t physical_address {};
    PageFlags flags {};
};

constexpr uint16_t paging_num_free_memory_regions = 16;

/**
 * @brief Allocate space for the new page table, and re-map essential mappings
 * mapped by Limine. Then free the old page table by reclaiming bootloader reclaimable
 * memory.
 */
void paging_init();

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
void paging_add_mapping(uintptr_t virtual_base,
                     uintptr_t physical_base,
                     uint64_t length,
                     PageFlags flags);

/**
 * @brief Allocate (not-necessarily contiguous) physical memory
 * for the specified contiguous virtual address region and map it in the page tree.
 */
auto paging_allocate_and_map(uintptr_t virtual_base, size_t length, PageFlags flags) -> void;

/**
 * @brief Get the virtual memory regions that are free to be managed (e.g. by an allocator)
 * after the initial reserved page mappings have been set up.
 * 
 * @return frg::array<MemoryRegion> 
 */
auto paging_get_initial_free_regions() -> dav::array<MemoryRegion, paging_num_free_memory_regions>;

/**
 * @brief Get information about the translation for a given virtual address.
 * 
 * @param virtual_address 
 * @return uintptr_t 
 */
auto paging_get_translation(uintptr_t virtual_address) -> PageTranslation;


/**
 * @brief Get the first and last pages for the smallest range containing a contiguous
 * virtual memory region.
 */
auto get_page_range(uintptr_t virtual_base, size_t length) -> PageRange;

#endif