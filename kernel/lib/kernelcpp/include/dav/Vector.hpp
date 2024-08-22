#ifndef KERNELCPP_VECTOR_HPP
#define KERNELCPP_VECTOR_HPP

namespace dav {

template<typename T>
class Vector {
public:
    Vector() = default;

    Vector(std::initializer_list<T> list)
    {
        resize(list.size());

    size_t size() const noexcept { return m_size; }
    size_t capacity() const noexcept { return m_capacity; }

private:
    T* m_data = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;
};

} // namespace dav
