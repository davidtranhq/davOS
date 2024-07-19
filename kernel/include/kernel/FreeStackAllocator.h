#ifndef DAVOS_KERNEL_FREE_STACK_ALLOCATOR_H_INCLUDED
#define DAVOS_KERNEL_FREE_STACK_ALLOCATOR_H_INCLUDED

#include <cstddef>
#include <dav/optional.hpp>
#include <dav/UserAllocatedStack.hpp>
#include <kernel/Allocator.h>
#include <kernel/kernel.h>

template <typename T, std::size_t block_size>
class FreeStackAllocator : public Allocator<FreeStackAllocator<T, block_size>>
{
public:
    FreeStackAllocator() = default;

    // FreeListAllocator(T *base, std::size_t size);

    auto allocate_impl(std::size_t request) -> T *;

    auto deallocate_impl(T *base, std::size_t size) -> void;

    auto add_memory_impl(T *base, std::size_t size) -> void;

    auto get_size_impl(T *base) -> std::size_t;

private:
    dav::optional<dav::UserAllocatedStack<T *>> free_stack;
};

#include <kernel/FreeStackAllocator.tpp>

#endif // DAVOS_KERNEL_FREE_STACK_ALLOCATOR_H_INCLUDED
