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
        || entry->type == LIMINE_MEMMAP_FRAMEBUFFER
        || entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE;
}

void vmm_init()
{
    /**
     * The Limine boot protocol initializes the following mappings:
     *  - identity map of the first 4 GiB starting at 0x1000
     *  - A virtual address V above the HHDM address maps to HHDM - V
     *      - the HHDM address is obtained through the Limine limine_hhdm feature
     *  - an additional HHDM + identity mapping for any additional entry in the Limine memory map
     *    (if not already covered by the first two mappings)
     *      - the memory map can be accessed through the limine_memmap feature
     *  - a mapping for the kernel, which can be obtained from the limine_kernel_address feature
     *
     * We create our own page table and fill it with the above mappings, then free the physical
     * frames associated with Limine's page table (so that we can manage virtual memory ourselves).
     *
     * For the official documentation of the limine boot protocol, see
     * https://github.com/limine-bootloader/limine/blob/v5.x-branch/PROTOCOL.md#entry-memory-layout
     * 
     */

#ifdef DEBUG_BUILD
    DEBUG("Initializing virtual memory manager...\n");
#endif

    // allocate memory for the root of the page tree
    auto pml4_table_frame = allocate_frame();

#ifdef DEBUG_BUILD
    DEBUG("Allocated PML4 table frame at (physical) %p\n", pml4_table_frame);
#endif

    // since paging is already enabled, we need to access the physical frame
    // by its virtual address
    auto virtual_pml4_table_frame = physical_to_limine_virtual(
        reinterpret_cast<uintptr_t>(pml4_table_frame));
    
    // construct the page tree, with space for the root node allocated
    //  at the specified virtual address
    page_tree.emplace(reinterpret_cast<void *>(virtual_pml4_table_frame));

#ifdef DEBUG_BUILD
    DEBUG("Constructed page tree at (virtual) %p\n", virtual_pml4_table_frame);
#endif
    
    // add initial 4 GiB identity map
    vmm_add_mapping(0x1000, 0x1000, 0x100000000, PageFlags::Write);

    // add 4 GiB HHDM map
    vmm_add_mapping(limine::hhdm_address->offset, 0x0, 0x100000000, PageFlags::Write);

    // add kernel mapping
    vmm_add_mapping(limine::kernel_address->virtual_base, limine::kernel_address->physical_base,
                    0x100000000, PageFlags::None);

    // add memory-map mappings from the limine_memmap feature
    for (size_t i = 0; i < limine::memory_map->entry_count; ++i)
    {
        struct limine_memmap_entry *entry = limine::memory_map->entries[i];
        PageFlags flags = PageFlags::Write;
        auto virtual_base = reinterpret_cast<uintptr_t>(
            physical_to_limine_virtual(entry->base));
        vmm_add_mapping(virtual_base, entry->base, entry->length, flags);
        vmm_add_mapping(virtual_base + limine::hhdm_address->offset,
            entry->base, entry->length, flags);
    }

    // load page table base register (PTBR) to point to the physical address of the page table
    load_ptbr(reinterpret_cast<uintptr_t>(pml4_table_frame));

#ifdef DEBUG_BUILD
    DEBUG("Loaded PTBR to point to %p\n", pml4_table_frame);
#endif

    free_limine_bootloader_memory();
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
    // account for overflow
    if (last_page < first_page)
    {
        last_page = page_floor(UINT64_MAX);
    }
    // address of the first and last frames containing the virtual memory region 
    uint64_t first_frame = page_floor(physical_base);
    for (uint64_t page = first_page, frame = first_frame;
         page < last_page;
         page += PAGE_SIZE, frame += FRAME_SIZE)
    {
        page_tree->map_page_to_frame(page, frame, flags);
    }

#ifdef DEBUG_BUILD
    uint64_t num_pages = (last_page - first_page) / PAGE_SIZE;
    DEBUG("Mapped %d virtual page(s) %x to %x (end-exclusive) to %d "
          "physical frame(s) starting at %x. %d physical frames left.\n",
          num_pages, first_page, last_page, num_pages, first_frame, available_frames());
#endif
}
