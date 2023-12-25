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
#include <kernel/types.h>

#include <string.h>

extern "C" void load_ptbr(uintptr_t page_table_physical_address);

#define PAGE_SIZE 0x1000
#define FRAME_SIZE 0x1000

// virtual address locations of the readonly and read/write boundaries
// of the kernel: specified by the linker
extern LinkerAddress kernel_readonly_start,
                     kernel_readonly_end,
                     kernel_rw_start,
                     kernel_rw_end;

frg::optional<PageTree> page_tree;

void add_initial_mappings()
{
    // add initial 4 GiB identity map
    vmm_add_mapping(0x1000, 0x1000, 0x100000000, PageFlags::Write);

    // add 4 GiB HHDM map
    vmm_add_mapping(limine::hhdm_address->offset, 0x0, 0x100000000, PageFlags::Write);

    // add kernel mapping (readonly section)
    uint64_t kernel_readonly_length = &kernel_readonly_end - &kernel_readonly_start;
    uintptr_t kernel_readonly_start_physical = limine::kernel_address->physical_base;
    vmm_add_mapping(reinterpret_cast<uintptr_t>(&kernel_readonly_start),
                    kernel_readonly_start_physical,
                    kernel_readonly_length,
                    PageFlags::None);
    DEBUG("Mapped read-only kernel section.\n");

    // add kernel mapping (read/write section)
    uint64_t kernel_rw_length = &kernel_rw_end - &kernel_rw_start;
    uintptr_t kernel_rw_start_physical = kernel_readonly_start_physical + kernel_readonly_length;
    vmm_add_mapping(reinterpret_cast<uintptr_t>(&kernel_rw_start),
                    kernel_rw_start_physical,
                    kernel_rw_length,
                    PageFlags::Write);
    DEBUG("Mapped read/write kernel section.\n");
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

    DEBUG("Initializing virtual memory manager...\n");

    // allocate memory for the root of the page tree
    auto pml4_table_frame = allocate_frame();
    DEBUG("Allocated PML4 table frame at (physical) %p\n", pml4_table_frame);

    // since paging is already enabled, we need to access the physical frame
    // by its virtual address
    auto virtual_pml4_table_frame = kernel_physical_to_virtual(pml4_table_frame);

    // construct the page tree, with space for the root node allocated
    //  at the specified virtual address
    page_tree.emplace(virtual_pml4_table_frame);
    DEBUG("Constructed page tree at (virtual) %p\n", virtual_pml4_table_frame);
    
    
    add_initial_mappings();
    char *before_page_table = reinterpret_cast<char *>(pml4_table_frame) - 20;
    memcpy(before_page_table, "david tran", 13);
    DEBUG("Added initial mappings.\n");

    // load page table base register (PTBR) to point to the physical address of the page table
    load_ptbr(reinterpret_cast<uintptr_t>(pml4_table_frame));
    DEBUG("Loaded PTBR to point to %p\n", pml4_table_frame);

    free_limine_bootloader_memory();
}

/**
 * @brief Round the address down to the nearest page.
 */
uint64_t page_floor(uint64_t address)
{
    return address - (address % PAGE_SIZE);
}

/**
 * @brief Round the address up to the nearest page,
 * or down if the nearest page would cause an overflow.
 */
uint64_t page_ceil(uint64_t address)
{
    if (address % PAGE_SIZE == 0)
        return address;
    // check for overflow
    if (UINT64_MAX - PAGE_SIZE < address)
        return address;
    return page_floor(address + PAGE_SIZE);
}

void vmm_add_mapping(uintptr_t virtual_base,
                     uintptr_t physical_base,
                     uint64_t length,
                     PageFlags flags)
{
    // addresss of the first and last page containing the virtual memory region
    const uint64_t first_page = page_floor(virtual_base);
    const uint64_t last_page = [&]{
        // check for overflow
        if (page_floor(UINT64_MAX) - length < virtual_base)
            return page_floor(UINT64_MAX);
        return page_ceil(virtual_base + length);
    }();
    // address of the first and last frames containing the virtual memory region 
    uint64_t first_frame = page_floor(physical_base);
    for (uint64_t page = first_page, frame = first_frame;
         page < last_page;
         page += PAGE_SIZE, frame += FRAME_SIZE)
    {
        page_tree->map_page_to_frame(page, frame, flags);
    }

    uint64_t num_pages = (last_page - first_page) / PAGE_SIZE;
    DEBUG("Mapped %d virtual page(s) %x to %x (end-exclusive) to %d "
          "physical frame(s) starting at %x. %d physical frames left.\n",
          num_pages, first_page, last_page, num_pages, first_frame, available_frames());
}
