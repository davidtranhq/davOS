#pragma once

#include <cstddef>

namespace kpp
{

/**
 * @brief A stack that does not own the memory it uses to store its stack contents:
 * a fixed memory allocation needs to be passed in through the constructor and managed externally.
 *
 * It also cannot grow; the size is fixed and determined when the stack is constructed.
 *
 * These restrictions exist to allow it to be used in an environnment where a dynamic
 * memory allocator doesn't yet exist.
 */
class NonOwningStack
{
public:
    NonOwningStack(void *base, size_t max_size) noexcept
        : m_stack{reinterpret_cast<void **>(base)},
          m_maxSize{max_size}
    {}

    void push(void *frame_pointer) noexcept
    {
        if (full())
            return;
        m_stack[m_size] = frame_pointer;
        m_size += 1;
    }

    void pop() noexcept
    {
        if (empty())
            return;
        m_size -= 1;
    }

    [[ nodiscard ]]
    void *top() const noexcept
    {
        return m_stack[m_size - 1];
    }

    [[ nodiscard ]]
    bool empty() const noexcept
    {
        return m_size == 0;
    }

    [[ nodiscard ]]
    bool full() const noexcept
    {
        return m_size == m_maxSize;
    }

    [[ nodiscard ]]
    size_t size() const noexcept
    {
        return m_size;
    }

private:
    void **m_stack = nullptr;
    std::size_t m_maxSize = 0;
    std::size_t m_size = 0;
};

} // namespace kpp
