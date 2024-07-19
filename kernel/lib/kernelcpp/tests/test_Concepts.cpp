#include <gtest/gtest.h>
#include <dav/type_traits.hpp>
#include <dav/concepts.hpp>

TEST(ConceptsAndTypeTraitsTest, SameAs)
{
    static_assert(dav::SameAs<int, int>);
    static_assert(!dav::SameAs<bool, int>);
    static_assert(!dav::SameAs<int&, int>);
    static_assert(!dav::SameAs<const int, int>);
    static_assert(!dav::SameAs<volatile int, int>);
    SUCCEED();
}

TEST(ConceptsAndTypeTraitsTest, RemoveReferenceAndCvQualifiers)
{
    static_assert(dav::SameAs<typename dav::RemoveReference<int&>::type, int>);
    static_assert(dav::SameAs<typename dav::RemoveCvQualifiers<const volatile int>::type, int>);
    static_assert(dav::SameAs<
        typename dav::RemoveReferenceAndCvQualifiers<const volatile int&>::type, 
        int
    >);
}
