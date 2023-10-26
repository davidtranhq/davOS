/*
limine page table maps kernel, framebuffer, and others (stack)
allocate physical frame for PML4 table frame (page tree root)
    convert physical address to virtual address mapped by Limine (is allocated frame guaranteed to be mapped?)
    add essential mappings to new page table, allocating frames as needed for each depth

*/

#include <frg/optional.hpp>
#include <kernel/vmm.h>
#include <kernel/frame_allocator.h>
#include <kernel/PageTree.h>
#include <kernel/kernel.h>
#include <kernel/limine_features.h>
#include <kernel/macros.h>

extern "C" void load_ptbr(uintptr_t page_table_physical_address);

#define PAGE_SIZE 0x1000
#define FRAME_SIZE 0x1000

frg::optional<PageTree> page_tree;

/**
 * @brief Check if an entry in the Limine memory map must be mapped in the page tables.
 */
bool is_essential_mapping(struct limine_memmap_entry *entry)
{
    return entry->type == LIMINE_MEMMAP_KERNEL_AND_MODULES
        || entry->type == LIMINE_MEMMAP_FRAMEBUFFER;
}

/**
 * @brief Allocate space for the new page table, and re-map essential mappings
 * mapped by Limine. Then free the old page table by reclaiming bootloader reclaimable
 * memory.
 */
void vmm_init()
{
#ifdef DEBUG_BUILD
    DEBUG("Initializing virtual memory manager...\n");
#endif

    auto pml4_table_frame = allocate_frame();

#ifdef DEBUG_BUILD
    DEBUG("Allocated PML4 table frame at (physical) %p\n", pml4_table_frame);
#endif

    // since paging is already enabled, we need to access the physical frame
    // by its virtual address
    auto virtual_pml4_table_frame = physical_to_limine_virtual(
        reinterpret_cast<uintptr_t>(pml4_table_frame));
    if (virtual_pml4_table_frame == 0)
        kernel_panic("allocate_frame_limine_virtual: \
            frame allocated to unmapped Limine virtual address");
    // construct the page tree, with space for the root node allocated
    //  at the specified virtual address
    page_tree.emplace(reinterpret_cast<void *>(virtual_pml4_table_frame));

#ifdef DEBUG_BUILD
    DEBUG("Constructed page tree at (virtual) %p\n", virtual_pml4_table_frame);
#endif
    
    // copy over essential mappings from the old page table to the new one
    for (size_t i = 0; i < limine::memory_map->entry_count; ++i)
    {
        struct limine_memmap_entry *entry = limine::memory_map->entries[i];
        if (is_essential_mapping(entry))
        {
            PageFlags flags = PageFlags::Write;
            auto virtual_base = reinterpret_cast<uintptr_t>(
                physical_to_limine_virtual(entry->base));
            vmm_add_mapping(virtual_base, entry->base, entry->length, flags);
        }
    }

    // load page table base register (PTBR) to point to the physical address of the page table
    load_ptbr(reinterpret_cast<uintptr_t>(pml4_table_frame));

#ifdef DEBUG_BUILD
    DEBUG("Loaded PTBR to point to %p\n", pml4_table_frame);
#endif

}

/**
 * @brief Round the address down to the nearest page.
 */
uint64_t page_floor(uint64_t address)
{
    return address - (address % PAGE_SIZE);
}

void vmm_add_mapping(uintptr_t virtual_base,
                     uintptr_t physical_base,
                     uint64_t length,
                     PageFlags flags)
{
    // addresss of the first and last page containing the virtual memory region
    uint64_t first_page = page_floor(virtual_base);
    uint64_t last_page = page_floor(virtual_base + length);
    // address of the first and last frames containing the virtual memory region 
    uint64_t first_frame = page_floor(physical_base);

    for (uint64_t page = first_page, frame = first_frame;
         page <= last_page;
         page += PAGE_SIZE, frame += FRAME_SIZE)
    {
        page_tree->map_page_to_frame(page, frame, flags);
    }
}
