#include <kernel/PageTreeNode.h>
#include <kernel/macros.h>

#include <stdint.h>

uintptr_t PageTreeNode::get_child_address(int index)
{
    uint64_t entry = entries_[index];
    // 32 bits starting at bit 12
    uintptr_t child_address = (entry & (0xffffffff << 12));
    return child_address;
}

void PageTreeNode::set_child_address(int index, uintptr_t address)
{
    uint64_t &entry = entries_[index];
    entry &= (~0xffffffff) << 12;
    entry |= address;
    // mark as present
    // 0x27 because this is what we observe in all the page frames in limine's page tables
    entry |= 0x27;
}

void PageTreeNode::add_child_flags(int index, PageFlags flags)
{
    uint64_t &entry = entries_[index];
    entry |= flags;
}