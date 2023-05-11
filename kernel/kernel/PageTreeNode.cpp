#include <kernel/PageTreeNode.h>

#include <stdint.h>

uintptr_t PageTreeNode::get_child_address(int index)
{
    uint64_t entry = entries_[index];
    // 32 bits starting at bit 12
    uintptr_t child_address = (entry >> 12) & 0xffffffff;
    return child_address;
}

void PageTreeNode::set_child_address(int index, uintptr_t address)
{
    uint64_t &entry = entries_[index];
    // 32 bits starting at bit 12
    uint64_t address_mask = (address & 0xffffffff) << 12;
    // first clear the bits
    entry &= ~address_mask;
    // then set them
    entry |= address_mask;
}

void PageTreeNode::add_child_flags(int index, PageFlags flags)
{
    uint64_t &entry = entries_[index];
    entry |= flags;
}