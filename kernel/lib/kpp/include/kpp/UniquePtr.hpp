#pragma once

#include <concepts>
#include <utility>

namespace kpp
{
template<typename Resource>
class UniquePtr
{
public:
    UniquePtr() = default;

    explicit UniquePtr(Resource* resource)
        : m_resource {resource}
    {};

    UniquePtr(const UniquePtr&) = delete;
    
    UniquePtr& operator=(const UniquePtr&) = delete;

    template<typename DerivedResource>
        requires std::convertible_to<DerivedResource*, Resource*>
    UniquePtr(UniquePtr<DerivedResource>&& source)
        : m_resource(source.release())
    {}

    template<typename DerivedResource>
        requires std::convertible_to<DerivedResource*, Resource*>
    UniquePtr<DerivedResource>& operator=(UniquePtr<DerivedResource>&& source)
    {
        if (m_resource)
            m_resource->~Resource();
        m_resource = source.release();
        return *this;
    }

    ~UniquePtr()
    { 
        if (m_resource)
            m_resource->~Resource();
    }

    Resource& operator*()
    {
        // TODO: checked access with something like kpp::expected ?
        return *m_resource;
    }

    Resource* operator->()
    { 
        // TODO: checked access with something like kpp::expected ?
        return m_resource;
    }

    const Resource* operator->() const
    {
        // TODO: checked access with something like kpp::expected ?
        return m_resource;
    }

    operator bool() const { return m_resource; }

    Resource* release() noexcept
    {
        auto* ptr = m_resource;
        m_resource = nullptr;
        return ptr;
    }

private:
    Resource* m_resource = nullptr;
};

template<typename Resource, typename ...ConstructorArguments>
UniquePtr<Resource> makeUnique(ConstructorArguments&&... args)
{
    return UniquePtr<Resource>(new Resource(std::forward<ConstructorArguments>(args)...));
}
} // namespace kpp
