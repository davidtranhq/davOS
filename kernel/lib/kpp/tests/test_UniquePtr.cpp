#include <kpp/UniquePtr.hpp>
#include <gtest/gtest.h>

#include <vector>

class ResourcePool {
public:
    class Resource {
    public:
        Resource(ResourcePool& source)
            : m_source(source)
        {
            m_source.trackResource();
        }

        ~Resource() { m_source.untrackResource(); }

    private:
        ResourcePool& m_source;
    };

    ~ResourcePool()
    {
        for (auto* resource : m_resources)
            delete resource;
    }

    Resource* makeResource() { return new Resource(*this); }
    void trackResource() { m_numResourcesAlive++; }
    void untrackResource() { m_numResourcesAlive--; }
    auto numResourcesAlive() const { return m_numResourcesAlive; }

private:
    int m_numResourcesAlive = 0;
    std::vector<Resource*> m_resources;
};

TEST(UniquePtrTest, ReleasesObjectOnDestruction)
{
    auto resourcePool = ResourcePool {};
    {
        [[maybe_unused]]
        auto ptr
            = kpp::UniquePtr(resourcePool.makeResource());
        EXPECT_EQ(resourcePool.numResourcesAlive(), 1);
    }
    EXPECT_EQ(resourcePool.numResourcesAlive(), 0);
}

TEST(UniquePtrTest, TransfersOwnershipOnMoveConstructor)
{
    auto resourcePool = ResourcePool {};
    {
        auto firstPtr = kpp::UniquePtr(resourcePool.makeResource());
        {
            [[maybe_unused]]
            auto secondPtr
                = std::move(firstPtr);
            EXPECT_EQ(resourcePool.numResourcesAlive(), 1);
        }
        EXPECT_EQ(resourcePool.numResourcesAlive(), 0);
    }
    EXPECT_EQ(resourcePool.numResourcesAlive(), 0);
}

TEST(UniquePtrTest, TransfersOwnershipOnMoveAssignment)
{
    auto resourcePool = ResourcePool {};
    {
        auto firstPtr = kpp::UniquePtr(resourcePool.makeResource());
        {
            [[maybe_unused]]
            auto secondPtr
                = kpp::UniquePtr<ResourcePool::Resource> {};
            secondPtr = std::move(firstPtr);
            EXPECT_EQ(resourcePool.numResourcesAlive(), 1);
        }
        EXPECT_EQ(resourcePool.numResourcesAlive(), 0);
    }
    EXPECT_EQ(resourcePool.numResourcesAlive(), 0);
}

TEST(UniquePtrTest, StoresDerivedClass)
{
    enum class Value { base,
        derived };

    struct Base {
        virtual Value foo() { return Value::base; }
        virtual ~Base() = default;
    };

    struct Derived : Base {
        Value foo() override { return Value::derived; }
    };

    kpp::UniquePtr<Base> basePtrContainingDerived = kpp::makeUnique<Derived>();
    EXPECT_EQ(basePtrContainingDerived->foo(), Value::derived);
}
