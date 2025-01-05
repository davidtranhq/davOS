#include <cstddef>

#include <kernel/FreeStackAllocator.h>
#include <kernel/kernel.h>

template <typename T, std::size_t block_size>
auto FreeStackAllocator<T, block_size>::allocate_impl(std::size_t request) -> T * {
    if (request > block_size) {
        kernel_panic("free stack allocator doesn't support contiguous allocations larger"
            "than the fixed block size (%x bytes)", request);
    }

    if (free_stack->empty()) {
        kernel_panic("ran out of physical memory to allocate!");
    }
    auto const frame_address = free_stack->top();
    free_stack->pop();
    return frame_address;
}

template <typename T, std::size_t block_size>
auto FreeStackAllocator<T, block_size>::deallocate_impl(T *ptr, size_t size) -> void {
    free_stack->push(ptr);
}

template <typename T, std::size_t block_size>
auto FreeStackAllocator<T, block_size>::add_memory_impl(T *ptr, size_t size) -> void {
    
}

