#ifndef KERNELCPP_FUNCTIONAL_HPP
#define KERNELCPP_FUNCTIONAL_HPP

#include <dav/UniquePtr.hpp>
#include <utility>

namespace dav {

template <typename T = void>
struct less {
    constexpr auto operator()(const T &lhs, const T &rhs) const {
        return lhs < rhs;
    }
};

template<typename>
class Function;

template<typename ReturnType, typename ...ArgumentTypes>
class Function<ReturnType(ArgumentTypes...)> {
public:
    class Callable {
    public:
        virtual ReturnType call(ArgumentTypes...) = 0;
        virtual ~Callable() = default;
    };

    template<typename CallableType>
    class CallableImpl : public Callable {
    public:
        template<typename T>
        CallableImpl(T&& callable) 
            : m_callable(forward<T>(callable))
        {}

        ReturnType call(ArgumentTypes... args) override
        {
            return m_callable(args...);
        }
    private:
        CallableType m_callable;
    };

    template<typename CallableType>
    Function(CallableType&& callable)
        : m_callable(makeUnique<CallableImpl<CallableType>>(forward<CallableImpl<CallableType>>(callable)))
    {}

    ReturnType operator()(ArgumentTypes... args)
    {
        assert(m_callable);
        return m_callable->call(args...);
    }


    UniquePtr<Callable> m_callable;
};

template<typename>
struct SignatureFromFunctionPointer
{};

// Ordinary function pointers
template<typename ReturnType, typename Functor, bool isNoexcept, typename ...Arguments>
struct SignatureFromFunctionPointer<ReturnType(Functor::*)(Arguments...) noexcept(isNoexcept)> {
    using type = ReturnType(Arguments...);
};

// Member functions on reference types
template<typename ReturnType, typename Functor, bool isNoexcept, typename ...Arguments>
struct SignatureFromFunctionPointer<ReturnType(Functor::*)(Arguments...) & noexcept(isNoexcept)> {
    using type = ReturnType(Arguments...);
};

// Member functions on const types
template<typename ReturnType, typename Functor, bool isNoexcept, typename ...Arguments>
struct SignatureFromFunctionPointer<ReturnType(Functor::*)(Arguments...) const noexcept(isNoexcept)> {
    using type = ReturnType(Arguments...);
};

// Member functions on const reference types
template<typename ReturnType, typename Functor, bool isNoexcept, typename ...Arguments>
struct SignatureFromFunctionPointer<ReturnType(Functor::*)(Arguments...) const & noexcept(isNoexcept)> {
    using type = ReturnType(Arguments...);
};

template<
    typename Functor, 
    typename Signature = typename SignatureFromFunctionPointer<decltype(&Functor::operator())>::type
>
Function(Functor) -> Function<Signature>;

} // namespace dav

#endif
