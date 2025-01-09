#pragma once

#include <concepts>
#include <kpp/UniquePtr.hpp>

namespace kpp {

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
private:
    class Callable {
    public:
        virtual ~Callable() = default;
        virtual ReturnType call(ArgumentTypes...) = 0;
        virtual UniquePtr<Callable> clone() const = 0;
    };

    template<typename CallableType>
    class CallableImpl : public Callable {
    public:
        template<typename T>
        CallableImpl(T&& callable) 
            : m_callable(std::forward<T>(callable))
        {}

        ReturnType call(ArgumentTypes... args) override
        {
            return m_callable(args...);
        }

        UniquePtr<Callable> clone() const override
        {
            return makeUnique<CallableImpl>(m_callable);
        }

    private:
        CallableType m_callable;
    };

public:
    Function() noexcept = default;

    Function(decltype(nullptr) ptr)
        : m_callable(ptr)
    {}

    Function(const Function& source)
        : m_callable(source.m_callable ? source.m_callable->clone() : UniquePtr<Callable>(nullptr))
    {}

    Function(Function&& source) = default;

    /*
     * Construct a dav::Function from an arbitrary callable type, that is any type for which
     * operator()() is defined. (Note that technically in the standard pointers to class members
     * are also defined as callable, but we don't support that yet).
     *
     * The template constraint ensures that the callable type is not itself a dav::Function,
     * to avoid shadowing the copy construcotr.
     */
    template<typename CallableType>
        requires (!std::same_as<std::remove_cvref_t<CallableType>, Function>)
    Function(CallableType&& callable) 
        : m_callable(makeUnique<CallableImpl<CallableType>>(std::forward<CallableType>(callable)))
    {}

    Function& operator=(const Function& source)
    {
        m_callable = source.m_callable.clone();
    }

    Function& operator=(Function&& source) = default;

    ReturnType operator()(ArgumentTypes... args)
    {
        // TODO: assert(m_callable) ?
        return m_callable->call(args...);
    }

private:
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

} // namespace kpp
