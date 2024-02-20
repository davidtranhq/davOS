/*
limine page table maps kernel, framebuffer, and others (stack)
allocate physical frame for PML4 table frame (page tree root)
    convert physical address to virtual address mapped by Limine (is allocated frame guaranteed to be mapped?)
    add essential mappings to new page table, allocating frames as needed for each depth

*/

#include <dav/algorithm.hpp>
#include <dav/optional.hpp>
#include <kernel/constants.h>
#include <kernel/paging.h>
#include <kernel/frame_allocator.h>
#include <kernel/PageTree.h>
#include <kernel/kernel.h>
#include <kernel/limine_features.h>
#include <kernel/macros.h>
#include <kernel/types.h>

#include <string.h>
#include <tuple>

extern "C" void load_ptbr(uintptr_t page_table_physical_address);

// virtual address locations of the readonly and read/write boundaries
// of the kernel: specified by the linker
extern LinkerAddress kernel_readonly_start,
                     kernel_readonly_end,
                     kernel_rw_start,
                     kernel_rw_end;


static dav::optional<PageTree> page_tree;

struct Mapping {
    MemoryRegion from_virtual {};
    uintptr_t to_physical {};
    PageFlags flags {};
};

static auto initial_mappings = dav::array<Mapping, 4> {{
    {
        // 4 GiB identity map
        .from_virtual {0x1000, 0x100000000},
        .to_physical {0x1000},
        .flags {PageFlags::Write}
    },
    {
        // 4 GiB HHDM map
        .from_virtual {limine::hhdm_address->offset, 0x100000000}, 
        .to_physical {0x0},
        .flags {PageFlags::Write}
    },
    {
        // kernel mapping (read-only section)
        .from_virtual {reinterpret_cast<uintptr_t>(&kernel_readonly_start),
            static_cast<size_t>(&kernel_readonly_end - &kernel_readonly_start)},
        .to_physical {limine::kernel_address->physical_base},
        .flags {PageFlags::None}
    },
    {
        // kernel mapping (read/write section)
        .from_virtual {reinterpret_cast<uintptr_t>(&kernel_rw_start),
            static_cast<size_t>(&kernel_rw_end - &kernel_rw_start)},
        .to_physical {limine::kernel_address->physical_base
            + (&kernel_readonly_end - &kernel_readonly_start)},
        .flags {PageFlags::Write}
    }
}};

/**
 * @brief Virtual ranges that are either already in use, or can't be addressed.
 */
static auto unavailable_virtual_memory_ranges = dav::array<MemoryRegion, initial_mappings.size() + 1> {{
    initial_mappings[0].from_virtual,
    {
        // non-canonical addresses can't be addressed
        .base {0x8000'0000'0000},
        .size {0xffff'0000'0000'0000}
    },
    initial_mappings[1].from_virtual,
    initial_mappings[2].from_virtual,
    initial_mappings[3].from_virtual
}};

void add_initial_mappings()
{
    for (const auto &[from_virtual, to_physical, flags] : initial_mappings) {
        paging_add_mapping(from_virtual.base, to_physical, from_virtual.size, flags);
    }
    DEBUG("Added initial mappings.\n");
}

void paging_init()
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
    return address - (address % page_size);
}

/**
 * @brief Round the address up to the nearest page,
 * or down if the nearest page would cause an overflow.
 */
uint64_t page_ceil(uint64_t address)
{
    if (address % page_size == 0)
        return address;
    // check for overflow
    // TODO: better more portable way to do this? relies on uint64_t
    if (UINT64_MAX - page_size < address)
        return address;
    return page_floor(address + page_size);
}

struct PageRange {
    uintptr_t first_page = 0;
    uintptr_t last_page = 0;
};

auto get_page_range(uintptr_t virtual_base, size_t length) -> PageRange
{
    const auto first_page = page_floor(virtual_base);
    const auto last_page = [&] {
        // check for overflow
        if (page_floor(UINT64_MAX) - length < virtual_base)
            return page_floor(UINT64_MAX);
        return page_ceil(virtual_base + length);
    }();
    return PageRange {first_page, last_page};
}

void paging_add_mapping(uintptr_t virtual_base,
                     uintptr_t physical_base,
                     uint64_t length,
                     PageFlags flags)
{
    // addresss of the first and last page containing the virtual memory region
    const auto [first_page, last_page] = get_page_range(virtual_base, length);
    // address of the first and last frames containing the virtual memory region 
    uint64_t first_frame = page_floor(physical_base);
    for (uint64_t page = first_page, frame = first_frame;
         page < last_page;
         page += page_size, frame += frame_size) 
    {
        page_tree->map_page_to_frame(page, frame, flags);
    }

    uint64_t num_pages = (last_page - first_page) / page_size;
    DEBUG("Mapped %d virtual page(s) %x to %x (end-exclusive) to %d "
          "physical frame(s) starting at %x. %d physical frames left.\n",
          num_pages, first_page, last_page, num_pages, first_frame, available_frames());
}

auto paging_allocate_and_map(uintptr_t virtual_base, size_t length, PageFlags flags) -> void
{
    const auto [first_page, last_page] = get_page_range(virtual_base, length);
    for (auto page = first_page; page != last_page; page += page_size) {
        paging_add_mapping(page, reinterpret_cast<uintptr_t>(allocate_frame()), page_size, flags);
    }
}

auto paging_get_initial_free_regions() -> dav::array<MemoryRegion, 16> {
    // dav::static_sort(begin(used_mappings), end(used_mappings),
    //     [](const auto &lhs, const auto &rhs) {
    //         return dav::tie(lhs.from_virtual.base, lhs.from_virtual.size, lhs.to_physical)
    //             < dav::tie(rhs.from_virtual.base, rhs.from_virtual.size, lhs.from_physical);
    //     });
    
    auto free_regions = dav::array<MemoryRegion, 16> {};
    auto i = std::size_t {0};
    auto base = uintptr_t {0x1000};
    for (const auto &[unavailable_base, unavailable_size]: unavailable_virtual_memory_ranges) {
        const auto unavailable_end = unavailable_base + unavailable_size;
        if (base >= unavailable_base) {
            base = dav::max(base, unavailable_end);
        } else {
            if (i > free_regions.size()) {
                kernel_panic("exceeded number of supported free regions (16)\n");
            }
            free_regions[i++] = {base, unavailable_base - base};
            base = unavailable_end;
        }
        // print unavailable
        DEBUG("Unavailable region: %x to %x\n", unavailable_base, unavailable_end);
    }
    return free_regions;
}

auto paging_get_translation(uintptr_t virtual_address) -> PageTranslation {
    return page_tree->get_translation(virtual_address);
}