#ifndef DAVOS_KERNELCPP_TYPE_TRAITS_HPP
#define DAVOS_KERNELCPP_TYPE_TRAITS_HPP

namespace dav
{

template<typename T, T v>
struct IntegralConstant {
    constexpr static T value = v;
    using value_type = T;
    constexpr operator value_type() { return value; }
};

using TrueType = IntegralConstant<bool, true>;
using FalseType = IntegralConstant<bool, false>;

template<typename T> struct IsLvalueReference : FalseType {};
template<typename T> struct IsLvalueReference<T&> : TrueType {};

template<typename T> struct RemoveReference { using type = T; };
template<typename T> struct RemoveReference<T&> { using type = T; };
template<typename T> struct RemoveReference<T&&> { using type = T; };

template<typename T> struct RemoveCvQualifiers { using type = T; };
template<typename T> struct RemoveCvQualifiers<const T> { using type = T; };
template<typename T> struct RemoveCvQualifiers<volatile T> { using type = T; };
template<typename T> struct RemoveCvQualifiers<const volatile T> { using type = T; };

template<typename T>
struct RemoveReferenceAndCvQualifiers : RemoveCvQualifiers<typename RemoveReference<T>::type>
{};

template<typename T>
using RemoveReferenceAndCvQualifiers_t = typename RemoveReferenceAndCvQualifiers<T>::type;

template<typename T> struct TypeIdentity { using type = T; };

/*
 * The difference between AddLvalueReference::type and T& is that T& will fail if T is a
 * non-referenceable type, like void.
 *
 * If the provided type T to AddLvalueReference is non-referenceable, T is returned.
 * Otherwise, T& is returned.
 */
namespace {
template<typename T>
concept LvalueReferenceable = requires { TypeIdentity<T&>::type; };

template<typename T>
concept RvalueReferenceable = requires { TypeIdentity<T&&>::type; };

template<typename T> requires LvalueReferenceable<T>
TypeIdentity<T&> tryAddLvalueReference();

template<typename T>
TypeIdentity<T> tryAddLValueReference();

template<typename T> requires RvalueReferenceable<T>
TypeIdentity<T&&> tryAddRvalueReference();

template<typename T>
TypeIdentity<T> tryAddRvalueReference();

} // anonymous namespace

template<typename T>
struct AddLValueReference : decltype(tryAddLvalueReference<T>()) {};

template<typename T>
struct AddRvalueReference : decltype(tryAddRvalueReference<T>()) {};

template<typename T, typename U>
struct IsSame : FalseType {};

template<typename T>
struct IsSame<T, T> : TrueType {};

} // namespace dav

#endif
