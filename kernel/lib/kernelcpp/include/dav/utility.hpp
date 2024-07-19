#ifndef DAVOS_KERNELCPP_UTILITY_HPP
#define DAVOS_KERNELCPP_UTILITY_HPP

#include <dav/type_traits.hpp>
#include <concepts>

namespace dav
{

/*
 * declval is used in generic template metaprogramming contexts to get the return value of
 * a member function on a type that can not be constructed.
 *
 * Suppose the following types:
 *
 * struct A { int foo(); };
 * struct B { int foo(); B() = delete; };
 *
 * To get the return type of A::foo, decltype(A().foo()) works.
 * But we can't get the return type of B::foo in the same way, since B::B() is deleted.
 * The important point is that we don't really need a constructed value of B, just
 * a way to reference one.
 *
 * Adding an rvalue reference to B works because of reference collapsing: the reference
 * types will be collapsed as follows:
 *      B -> &&B
 *     &B -> &B
 *    &&B -> &&B
 *
 * So declval<T>() simply adds an rvalue reference to T, and we can get the return type
 * of B::foo() with
 *
 *      decltype(declval<B>().foo())
 *
 */
template<typename T>
typename AddRvalueReference<T>::type declval() noexcept {
    static_assert(sizeof(T) && false, "declval is not allowed in an evaluated context");
}

/*
 * Produces an xvalue expression identifying the argument of move. Exactly equivalent
 * to a `static_cast` of `T` to an rvalue reference type.
 */
template<typename T>
constexpr RemoveReference<T>::type&& move(T&& value)
{
    return static_cast<typename RemoveReference<T>::type&&>(value);
}

/*
 * Converts non-reference types and rvalue references to rvalue references, and otherwise
 * leaves lvalue references as lvalue references.
 */
template<typename T>
constexpr T&& forward(typename RemoveReference<T>::type& value)
{
    return static_cast<T&&>(value);
}

/*
 * Overload forward for rvalue references to prevent creating an lvalue reference to an rvalue,
 * for example, `forward<int&>(5)`.
 */
template<typename T> requires (!LvalueReference<T>)
constexpr T&& forward(typename RemoveReference<T>::type&& value)
{
    return static_cast<T&&>(value); 

}

} // namespace dav

#endif
