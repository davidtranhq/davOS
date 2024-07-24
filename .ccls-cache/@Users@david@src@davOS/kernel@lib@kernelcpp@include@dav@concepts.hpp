#ifndef DAVOS_KERNELCPP_CONCEPTS_HPP
#define DAVOS_KERNELCPP_CONCEPTS_HPP

#include <dav/type_traits.hpp>

namespace dav
{
/*
 * A type `From` is defined as **convertible** to a type `To` if the imaginary function
 * definition `To test() { return declval<From>(); }` is well-formed.
 */
template<typename From, typename To>
concept ImplicitConvertible = requires(void (&functionAcceptingTo)(To), From from) {
    functionAcceptingTo(from);
};

template<typename T>
concept Returnable = requires {
    [] () -> T {};
};

template<typename From, typename To>
concept Convertible = ImplicitConvertible<From, To> && Returnable<To>;

template<typename T>
concept LvalueReference = IsLvalueReference<T>::value;

template<typename T, typename U>
concept SameAs = IsSame<T, U>::value;

} // namespace dav

#endif
