#ifndef KERNELCPP_ALLOCATED_VECTOR_HPP
#define KERNELCPP_ALLOCATED_VECTOR_HPP

namespace dav
{

#include <cstddef>

/**
 * @brief A vector that does not own the memory it uses to store its contents:
 * a fixed memory allocation needs to be passed in through the constructor and managed externally.
 */
template <typename T>
class allocated_vector {
public:
    allocated_vector(T *base, size_t max_size) noexcept
        : data_ {base},
          max_size_ {max_size}
    {}

    void push_back(T element) noexcept
    {
        if (full())
            return;
        data_[size_] = element;
        size_ += 1;
    }

    void pop_back() noexcept
    {
        if (empty())
            return;
        size_ -= 1;
    }

    T back() const noexcept
    {
        return data_[size_ - 1];
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

    T &operator[](size_t index) noexcept
    {
        return data_[index];
    }

private:
    T *data_;
    size_t size_ {0};
    size_t max_size_;
};

} // namespace dav

#endif