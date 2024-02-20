#ifndef DAVOS_KERNEL_PAGETREE_H_INCLUDED
#define DAVOS_KERNEL_PAGETREE_H_INCLUDED

#include <cstdint>

#include <kernel/PageTreeNode.h>

class PageTree
{
public: 
    enum class Depth : uint8_t
    {
        PML4 = 0,
        DirectoryPointer = 1,
        Directory = 2,
        Table = 3,
    };

    /**
     * @brief Construct a new Page Tree object, with space allocated at the 
     * given virtual address
     */
    PageTree(void *virtual_address);

    /**
     * @brief Map a virtual memory page to a physical frame in the page tree.
     *
     * @param page base address of the page to map
     * @param frame base address of the frame to map
     */
    void map_page_to_frame(uint64_t page, uint64_t frame, PageFlags flags);

    /**
     * @brief Get information about the physical translation for a given virtual address.
     * 
     * @param virtual_address 
     * @return PageTranslation 
     */
    auto get_translation(uint64_t virtual_address) -> PageTranslation;

private:
    PageTreeNode *root_ = nullptr;

};

#endif