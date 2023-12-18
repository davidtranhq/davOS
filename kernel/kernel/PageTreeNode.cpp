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
}

void PageTreeNode::add_child_flags(int index, PageFlags flags)
{
    uint64_t &entry = entries_[index];
    // add flags (ensure that the present bit is set)
    entry |= (flags | PageFlags::Present);
}

void PageTreeNode::set_child_flags(int index, PageFlags flags)
{
    uint64_t &entry = entries_[index];
    // clear flags
    entry &= ~(0xff);
    add_child_flags(index, flags);
}