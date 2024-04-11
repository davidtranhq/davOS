#ifndef KERNELCPP_OPTIONAL
#define KERNELCPP_OPTIONAL

#include <cstddef>
#include <new>

#include <dav/array.hpp>

namespace dav {



template <typename T>
class optional {
public:
    /**
     * @brief Construct an optional with no value.
     */
    constexpr optional() noexcept = default;

    /**
     * @brief Destroys the contained value by calling its destructor.
     * If there is no contained value, there is no effect. 
     */
    constexpr auto reset() noexcept -> void {
        if (is_present) {
            interpret_val()->~T();
            is_present = false;
        }
    }

    /**
     * @brief Direct initializes the contained value with the given arguments.
     * If a value already exists, the old value is destroyed by calling its destructor.
     * 
     * @param args the arguments to pass to the constructor
     */
    template <typename ...Args>
    constexpr auto emplace(Args &&...args) -> void {
        if (is_present) {
            reset();
        }
        new (buffer.data()) T(std::forward<Args>(args)...);
        is_present = true;
    }

    /**
     * @brief Return a const pointer to the contained value.
     * 
     * @return constexpr const T* 
     */
    constexpr auto operator->() const noexcept -> const T * {
        return interpret_val();
    }

    /**
     * @brief Return a pointer to the contained value.
     * 
     * @return constexpr T* 
     */
    constexpr auto operator->() noexcept -> T * {
        return interpret_val();
    }

    /**
     * @brief Return true/false whether the optional contains a value. 
     * 
     * @return true 
     * @return false 
     */
    constexpr auto operator!() const noexcept -> bool {
        return !is_present;
    }

private:
    /**
     * @brief Interpret the buffer as a (const) T and return a pointer to it.
     */
    constexpr auto interpret_val() const -> const T * {
        return std::launder(reinterpret_cast<const T *>(buffer.data()));
    }
    constexpr auto interpret_val() -> T * {
        return std::launder(reinterpret_cast<T *>(buffer.data()));
    }

    alignas(T) dav::array<std::byte, sizeof(T)> buffer {};
    bool is_present {false};
};

}

#endif // KERNELCPP_OPTIONAL