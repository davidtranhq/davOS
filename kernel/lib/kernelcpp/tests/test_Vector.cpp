#include <gtest/gtest.h>
#include <dav/vector.hpp>
#include <dav/concepts.hpp>
#include <utility>

/*template<typename T>*/
/*class TrackedValue {*/
/*public:*/
/*    template<typename U>*/
/*        requires (!SameAs<RemoveReferenceAndCvQualifiers_t<U, TrackedValue>)*/
/*    TrackedValue(U&& value)*/
/*        : m_value(value)*/
/*    {}*/
/**/
/*    TrackedValue(const TrackedValue& rhs)*/
/*        : m_value(rhs.m_value)*/
/*    { s_numCopies += 1; }*/
/**/
/*    TrackedValue(TrackedValue&& rhs)*/
/*        : m_value(dav::move(rhs.m_value))*/
/*    { s_numMoves += 1; }*/
/**/
/*    TrackedValue& operator=(const TrackedValue& rhs)*/
/*    {*/
/*        m_value = rhs.m_value;*/
/*        s_numCopies += 1;*/
/*    }*/
/**/
/*    TrackedValue& operator=(TrackedValue&& rhs)*/
/*    {*/
/*        m_value = dav::move(rhs.m_value);*/
/*        s_numMoves += 1;*/
/*    }*/
/**/
/*    static int numCopies() const { return s_numCopies; }*/
/*    static int numMoves() const { return s_numMoves; }*/
/**/
/*private:*/
/*    T m_value;*/
/*    static int s_numCopies = 0;*/
/*    static int s_numMoves = 0;*/
/*};*/

TEST(VectorTest, ConstructsEmpty)
{
    auto v = dav::Vector<int> {};
    EXPECT_EQ(v.size(), 0);
    EXPEC_EQ(v.capacity(), 0);
}

TEST(VectorTest, ConstructsFromInitializerList)
{
    auto v = dav::Vector<int> {1, 2, 3};
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}

TEST(VectorTest, DeducesTemplateArgument)
{
    auto v = dav::Vector {1, 2, 3};
    static_assert(SameAs<v::value_type, int>);
}

TEST(VectorTest, PushesAndPopsBack)
{
    auto v = dav::Vector<int> {};
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    v.pop_back();
    EXPECT_EQ(v.size(), 2);
    v.pop_back();
    EXPECT_EQ(v.size(), 1);
    v.pop_back();
    EXPECT_EQ(v.size(), 0);
    v.push_back(10);
    EXPECT_EQ(v[0], 10);
}

TEST(VectorTest, CopyConstructs)
{
    auto source = dav::Vector<TrackedValue<int>> {1, 2, 3};
    auto destination = source;
    EXPECT_EQ(destination.size(), 3);
    EXPECT_EQ(destination[0], 1);
    EXPECT_EQ(destination[1], 2);
    EXPECT_EQ(destination[2], 3);
}

TEST(VectorTest, CopyAssigns)
{
    auto source = dav::Vector {1, 2, 3};
    auto destination = dav::Vector<int> {};
    destination = source;
    EXPECT_EQ(destination.size(), 3);
    EXPECT_EQ(destination[0], 1);
    EXPECT_EQ(destination[1], 2);
    EXPECT_EQ(destination[2], 3);
}

