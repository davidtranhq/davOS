#include <gtest/gtest.h>
#include <dav/Any.hpp>
#include <string>

TEST(AnyTest, DefaultConstructs)
{
    dav::Any a;
    EXPECT_EQ(a.has_value(), false);
}

TEST(AnyTest, StoresAnyValue)
{
    dav::Any a;
    a = 42;
    EXPECT_EQ(a.get<int>(), 42);
    a = std::string {"hello"};
    EXPECT_EQ(a.get<std::string>(), std::string {"hello" });
}

TEST(AnyTest, ThrowsOnWrongType)
{
    dav::Any a;
    a = 2.0;
    try {
        auto x = a.get<int>();
    } catch (dav::BadCast const& c) {
        SUCCEED();
    }
    FAIL();
}
