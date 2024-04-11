#ifndef DAVOS_KERNEL_ALLOCATOR_H_INCLUDED
#define DAVOS_KERNEL_ALLOCATOR_H_INCLUDED

#include <cstddef>

using std::size_t;

/**
 * @brief CRTP base class for an allocator of type T.
 * 
 * @tparam Derived The derived class implementation.
 * @tparam T The type allocated by this allocator.
 */
template <typename Derived>
class Allocator;

template <template <typename> class Derived, typename T>
class Allocator<Derived<T>> {
public:
    using value_type = T;

    auto allocate(size_t size) -> T * {
        return static_cast<Derived<T> *>(this)->allocate_impl(size);
    }

    auto deallocate(T *ptr, size_t size = 0) -> void {
        static_cast<Derived<T> *>(this)->deallocate_impl(ptr, size);
    }

    /**
     * @brief Add memory to be managed by the allocator. 
     * 
     * @param ptr 
     * @param size 
     */
    auto add_memory(T *ptr, size_t size) -> void {
        static_cast<Derived<T> *>(this)->add_memory_impl(ptr, size);
    }

    /**
     * @brief Get the size of the region allocated at ptr.
     * 
     * @param ptr Pointer pointing to the allocated region. 
     * @return size_t Size of the allocated region.
     */
    auto get_size(T *ptr) -> size_t {
        return static_cast<Derived<T> *>(this)->get_size_impl(ptr);
    }
};

#endif
