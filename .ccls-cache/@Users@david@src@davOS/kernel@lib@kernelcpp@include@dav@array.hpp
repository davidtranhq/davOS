#ifndef KERNELCPP_ARRAY
#define KERNELCPP_ARRAY

#include <cstddef>
#include <utility>

namespace dav {

template <typename T, size_t N>
struct Array {
    using iterator = T *;
    using const_iterator = const T *;
    T arr[N];

    constexpr auto operator[](size_t i) noexcept -> T & {
        return arr[i];
    }

    constexpr auto operator[](size_t i) const noexcept -> const T & {
        return arr[i];
    }

    constexpr auto size() const noexcept -> size_t {
        return N;
    }

    auto data() noexcept -> T * {
        return arr;
    }

    auto data() const noexcept -> const T * {
        return arr;
    }

    constexpr auto begin() noexcept -> iterator {
        return arr;
    }

    constexpr auto begin() const noexcept -> const_iterator {
        return arr;
    }

    constexpr auto end() noexcept -> iterator {
        return arr + N;
    }

    constexpr auto end() const noexcept -> const_iterator {
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
constexpr auto array_of(T &&...t) -> Array<V, sizeof...(T)> {
    return {{std::forward<T>(t)...}};
}

} // namespace dav

#endif // KERNELCPP_ARRAY
