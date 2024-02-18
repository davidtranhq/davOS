#ifndef DAVOS_KERNEL_BUDDY_ALLOCATOR_H_INCLUDED
#define DAVOS_KERNEL_BUDDY_ALLOCATOR_H_INCLUDED

#include <stdint.h>

class VirtualMemoryAllocator
{
public:
    /**
     * @brief Construct a new Buddy Allocator with available memory space
     * starting at the given address and with the given size.
     * 
     * @param start_address 
     * @param size 
     */
    VirtualMemoryAllocator(void *start_address, uint64_t size);

    /**
     * @brief Allocate a contiguous block of memory at least as big as the given size.
     * 
     * @param size 
     * @return void* 
     */
    auto allocate(uint64_t size) -> void *;

    /**
     * @brief Deallocate a block of memory. 
     * 
     * @param ptr 
     */
    auto deallocate(void *ptr) -> void;

private:
    struct FreeBlock
    {
        FreeBlock *next = nullptr;
        void *address = nullptr;
        uint8_t order = 0;
    };

    struct FreeBlockList 
    {
        FreeBlock *head = nullptr;
        uint64_t *used_bitmap = nullptr;
    };

    constexpr static uint8_t max_order = 11;
    FreeBlockList free_block_lists[max_order] = {};
};

#endif