#pragma once

#include <cstddef>
#include <new>

#include <dav/Array.hpp>
#include <dav/concepts.hpp>

namespace dav {

struct Nullopt_t {
    explicit constexpr Nullopt_t(int) noexcept {}
};

template <typename T>
class Optional {
public:
    /**
     * @brief Construct an Optional with no value.
     */
    constexpr Optional() noexcept = default;

    /**
     * @brief Destroys the contained value by calling its destructor.
     * If there is no contained value, there is no effect. 
     */
    constexpr void reset() noexcept
    {
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
    constexpr void emplace(Args &&...args)
    {
        if (is_present) {
            reset();
        }
        new (buffer.data()) T(std::forward<Args>(args)...);
        is_present = true;
    }

    /**
     * @brief If *this contains a value before this call, destroy that value.
     */
    constexpr Optional& operator=(Nullopt_t) noexcept
    {
        reset();
        return *this;
    }

    
    /**
     * @brief Copy-construct an Optional with an value.
     * TODO: requires IsCopyConstructible<T> and IsCopyAssignable<T>
     *
     * @param value the value with which to construct the Optional
     */
    constexpr Optional& operator=(const Optional &other)
    {
        if (other.is_present) {
            emplace(*other);
        } else {
            reset();
        }
        return *this;
    }

    /**
     * @brief Move-construct an Optional with an rvalue.
     * TODO: requires IsMoveConstructible<T> and IsMoveAssignable<T>
     *
     * @param value the value with which to construct the Optional
     */
    constexpr Optional& operator=(Optional &&other) noexcept
    {
        if (other.is_present) {
            emplace(std::move(*other));
        } else {
            reset();
        }
        return *this;
    }

    /**
     * @brief Construct an Optional with a perfectly-forwarded value.
     *
     * @param value the value with which to construct the Optional
     */
    template<typename U = T> // default to using T if type deduction fails
    constexpr Optional& operator=(U&& value)
        requires (!SameAs<RemoveReferenceAndCvQualifiers_t<U>, Optional>)
    {
        if (is_present) {
            reset();
        }
        emplace(std::forward<U>(value));
        return *this;
    }
    
    // TODO: copy assignment and move assignment for optional<U> where U is convertible to T

    /**
     * @brief Return a reference to the contained value.
     * 
     * @return constexpr T& 
     */
    constexpr T& operator*() noexcept
    {
        return *interpret_val();
    }

    /**
     * @brief Return a const reference to the contained value.
     * 
     * @return constexpr const T& 
     */
    constexpr const T& operator*() const noexcept
    {
        return *interpret_val();
    }

    /**
     * @brief Return a const pointer to the contained value.
     * 
     * @return constexpr const T* 
     */
    constexpr const T* operator->() const noexcept
    {
        return interpret_val();
    }

    /**
     * @brief Return a pointer to the contained value.
     * 
     * @return constexpr T* 
     */
    constexpr T* operator->() noexcept
    {
        return interpret_val();
    }

    /**
     * @brief Return true/false whether the Optional contains a value. 
     * 
     * @return true 
     * @return false 
     */
    constexpr explicit operator bool() const noexcept
    {
        return is_present;
    }

    /**
     * @brief Return true/false whether the Optional contains a value. 
     * 
     * @return true 
     * @return false 
     */
    constexpr bool operator!() const noexcept
    {
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

    alignas(T) dav::Array<std::byte, sizeof(T)> buffer {};
    bool is_present {false};
};

}
