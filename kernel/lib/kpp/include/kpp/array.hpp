#pragma once

#include <cstddef>
#include <utility>

namespace kpp {

template <typename T, size_t N>
struct Array
{
    using iterator = T*;
    using const_iterator = const T*;
    T arr[N];

    constexpr T& operator[](size_t i) noexcept
    {
        return arr[i];
    }

    constexpr const T& operator[](size_t i) const noexcept
    {
        return arr[i];
    }

    [[ nodiscard ]]
    constexpr std::size_t size() const noexcept
    {
        return N;
    }

    T* data() noexcept
    {
        return arr;
    }

    const T* data() const noexcept
    {
        return arr;
    }

    constexpr iterator begin() noexcept
    {
        return arr;
    }

    constexpr const_iterator begin() const noexcept
    {
        return arr;
    }

    constexpr iterator end() noexcept
    {
        return arr + N;
    }

    constexpr const_iterator end() const noexcept
    {
        return arr + N;
    }
};

/**
 * @brief Direct-initializes an array from a parameter pack.
 * 
 * @tparam V 
 * @tparam T 
 * @param t 
 * @return array<V, sizeof...(T)> 
 */
template <typename V, typename ...T>
constexpr auto array_of(T &&...t) -> Array<V, sizeof...(T)>
{
    return {{std::forward<T>(t)...}};
}

} // namespace kpp
