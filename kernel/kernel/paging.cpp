#include <kernel/limine_features.h>
#include <new> // placement new

#include <kernel/paging.h>
#include <kernel/frame_allocator.h>
#include <kernel/PML4Table.h>

extern "C" void load_ptbr(uint64_t page_table_physical_address);

/**
 * @brief Check if an entry in the Limine memory map must be mapped in the page tables.
 */
bool is_essential_mapping(struct limine_memmap_entry *entry)
{
    return entry->type == LIMINE_MEMMAP_KERNEL_AND_MODULES
        || entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE
        || entry->type == LIMINE_MEMMAP_FRAMEBUFFER;
}

/**
 * @brief Allocate space for the new page table, and copy over the old page table
 * initialized by Limine.
 */
void paging_init()
{
    auto pml4_table_frame = allocate_frame();
    // since paging is already enabled, we need to access the physical frame
    // by its virtual address
    auto virtual_pml4_table_frame = physical_to_limine_virtual(pml4_table_frame);
    // allocate space for the page table at the specified virtual address
    PML4Table *pml4_table = new(reinterpret_cast<void *>(virtual_pml4_table_frame)) PML4Table;
    
    // copy over essential mappings from the old page table to the new one
    for (size_t i = 0; i < limine::memory_map->entry_count; ++i)
    {
        struct limine_memmap_entry *entry = limine::memory_map->entries[i];
        if (is_essential_mapping(entry))
        {
            PML4Table::Flags flags = PML4Table::Flags::Write;
            pml4_table->add_mapping(entry->base, entry->length, flags);
        }
    }

    // load page table base register (PTBR) to point to the physical address of the page table
    load_ptbr(pml4_table_frame);
}