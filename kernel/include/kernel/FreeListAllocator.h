#pragma once
#include <cstddef>
#include <kernel/Allocator.h>
#include <kernel/kernel.h>

template <typename T>
class FreeListAllocator : public Allocator<FreeListAllocator<T>>
{
public:
    FreeListAllocator() = default;
    
    auto allocate_impl(std::size_t request) -> T *;
    
    auto deallocate_impl(T *base, std::size_t size) -> void;

    auto add_memory_impl(T *base, std::size_t size) -> void;

    auto get_size_impl(T *base) -> std::size_t;

private:
    struct FreeBlockHeader;
    struct UsedBlockHeader;

    FreeBlockHeader *head = nullptr;
};

#include <kernel/FreeListAllocator.cpp> 
