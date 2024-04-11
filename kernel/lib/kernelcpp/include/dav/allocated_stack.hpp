#ifndef KERNELCPP_ALLOCATED_STACK_HPP
#define KERNELCPP_ALLOCATED_STACK_HPP

namespace dav
{

#include <cstddef>

/**
 * @brief A stack that does not own the memory it uses to store its stack contents:
 * a fixed memory allocation needs to be passed in through the constructor and managed externally.
 *
 * It also cannot grow; the size is fixed and determined when the stack is constructed.
 *
 * These restrictions exist to allow it to be used in an environnment where a dynamic
 * memory allocator doesn't yet exist.
 */
class allocated_stack
{
public:
    allocated_stack(void *base, size_t max_size) noexcept
        : stack_{reinterpret_cast<void **>(base)},
          max_size_{max_size}
    {}

    void push(void *frame_pointer) noexcept
    {
        if (full())
            return;
        stack_[size_] = frame_pointer;
        size_ += 1;
    }

    void pop() noexcept
    {
        if (empty())
            return;
        size_ -= 1;
    }

    void *top() const noexcept
    {
        return stack_[size_ - 1];
    }

    bool empty() const noexcept
    {
        return size_ == 0;
    }

    bool full() const noexcept
    {
        return size_ == max_size_;
    }

    size_t size() const noexcept
    {
        return size_;
    }

private:
    void **stack_ = 0;
    size_t max_size_ = 0;
    size_t size_ = 0;
};

#endif

}