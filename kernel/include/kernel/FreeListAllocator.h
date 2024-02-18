#ifndef DAVOS_KERNEL_FREE_LIST_ALLOCATOR_H_INCLUDED
#define DAVOS_KERNEL_FREE_LIST_ALLOCATOR_H_INCLUDED

#include <cstddef>
#include <kernel/Allocator.h>
#include <kernel/kernel.h>

template <typename T>
class FreeListAllocator : public Allocator<FreeListAllocator<T>>
{
public:
    FreeListAllocator() = default;

    // FreeListAllocator(T *base, std::size_t size);
    
    auto allocate_impl(std::size_t request) -> T *;
    
    auto deallocate_impl(T *base, std::size_t size) -> void;

    auto add_memory_impl(T *base, std::size_t size) -> void;

// private:
    struct FreeBlockHeader;
    struct UsedBlockHeader;

    FreeBlockHeader *head = nullptr;
};

#include <kernel/FreeListAllocator.tpp> 

#endif // DAVOS_KERNEL_FREE_LIST_ALLOCATOR_H_INCLUDED
