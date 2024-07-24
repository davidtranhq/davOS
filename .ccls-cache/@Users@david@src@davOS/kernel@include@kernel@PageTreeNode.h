#ifndef DAVOS_KERNEL_PAGETREENODE_H_INCLUDED
#define DAVOS_KERNEL_PAGETREENODE_H_INCLUDED

#include <stdint.h>

#include <kernel/paging.h>

class PageTreeNode
{
public:
    /**
     * @brief Get the virtual address of a child entry of this entry
     * 
     * @param index 0-511: the index of the child
     */
    uintptr_t get_child_address(int index);

    /**
     * @brief Set the virtual address for a specific entry
     *
     * @param index 0-511: the index of the child
     */
    void set_child_address(int index, uintptr_t address);

    /**
     * @brief Add to the flags of a specific entry
     * 
     * @param index 0-511: the index of the child
     * @param flags 
     */
    void add_child_flags(int index, PageFlags flags);

    /**
     * @brief Set the flags of a specific entry 
     * 
     * @param index 0-511: the index of the child
     * @param flags
     */
    void set_child_flags(int index, PageFlags flags);

    /**
     * @brief Get the flags of a specific entry
     * 
     * @param index 0-511: the index of the child
     */
    auto get_child_flags(int index) -> PageFlags;

private:
    // fill with zeros
    uint64_t entries_[512] = {};
};

#endif