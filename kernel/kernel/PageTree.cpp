#include <new>
#include <stdint.h>

#include <kernel/PageTree.h>
#include <kernel/kernel.h>
#include <kernel/macros.h>
#include <kernel/frame_allocator.h>

/**
 * @brief Get the page tree table index for the given depth from the virtual address.
 */
uint16_t get_table_index(uint64_t virtual_address, uint8_t depth)
{
    uint8_t index_start = 0;
    // table index is located in different parts of the virtual address depending on the depth
    switch (depth)
    {
    case 0:
        index_start = 39;
        break;
    case 1:
        index_start = 30;
        break;
    case 2:
        index_start = 21;
        break;
    case 3:
        index_start = 12;
        break;
    default:
        kernel_panic("unrecognized page tree depth\n");
        break;
    }
    uint16_t table_index = (virtual_address >> index_start) & 0x1ff; // mask lowest 9 bits
    return table_index;
}

PageTree::PageTree(void *virtual_address)
{
    // placement new: construct the PageTreeNode at virtual_address
    root_ = new(virtual_address) PageTreeNode;
}

void PageTree::map_page_to_frame(uint64_t page, uint64_t frame, PageFlags flags)
{
    PageTreeNode *curr = root_;
    const int max_depth = 3;
    // traverse the tree until curr is in the last level (page table level)
    for (int depth = 0; depth < max_depth; ++depth)
    {
        auto child_index = get_table_index(page, depth);
        auto next = reinterpret_cast<PageTreeNode *>(curr->get_child_address(child_index));
        // allocate a new node for the next level if unallocated
        if (!next)
        {
            auto new_node_frame = allocate_frame();
            auto new_node_frame_virtual = physical_to_limine_virtual(
                reinterpret_cast<uintptr_t>(new_node_frame));
            // construct the new page tree node at the given address
            auto new_node = new(new_node_frame_virtual) PageTreeNode;
            curr->set_child_address(child_index, reinterpret_cast<uintptr_t>(new_node));
            curr->add_child_flags(child_index, flags);
            next = reinterpret_cast<PageTreeNode *>(new_node_frame);
        }
        // move to the next depth
        curr = next;
    }

    // set the address and flags of the physical frame in the page table
    auto child_index = get_table_index(page, max_depth);
    curr->set_child_address(child_index, frame);
    curr->add_child_flags(child_index, flags);
}