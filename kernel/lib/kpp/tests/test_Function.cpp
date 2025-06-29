#include <kpp/functional.hpp>
#include <functional>
#include <gtest/gtest.h>

int foo() { return 2; }

TEST(FunctionTest, CallsFunctionPointer)
{
    auto f = kpp::Function<int(void)>(foo);
    EXPECT_EQ(f(), 2);
}

TEST(FunctionTest, CallsNonCapturingLambda)
{
    // note that this decays to a function pointer anyway
    auto lambda = [](int x) { return x; };
    auto f = kpp::Function(lambda);
    EXPECT_EQ(f(2), 2);
}

TEST(FunctionTest, CallsCapturingLambda)
{
    int x = 2;
    auto capturingLambda = [&]() { return x; };
    auto f = kpp::Function(capturingLambda);
    EXPECT_EQ(f(), 2);
}

TEST(FunctionTest, CallsFunctor)
{
    int x = 2;
    struct Functor {
        Functor(int x)
            : data(x)
        {
        }
        int operator()() { return data; }
        int data = 0;
    };

    auto functor = Functor(x);
    auto f = kpp::Function(functor);
    EXPECT_EQ(f(), 2);
}

TEST(FunctionTest, CopiesCallable)
{
    class FunctorWithState {
    public:
        int operator()(bool increment)
        {
            if (increment)
                x++;
            return x;
        }

    private:
        int x = 1;
    };
    auto source = kpp::Function(FunctorWithState {});
    auto destination = source;
    EXPECT_EQ(source(false), 1);
    EXPECT_EQ(destination(false), 1);
    EXPECT_EQ(source(true), 2);
    EXPECT_EQ(destination(true), 2);
}
