#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tests.h>

namespace
{

class GlobalConstructorTest
{
public:
    GlobalConstructorTest() : test_value_ {0xdeadbeef} {}

    bool was_constructed() const
    {
        return test_value_ == 0xdeadbeef;
    }

private:
    uint64_t test_value_;
};

GlobalConstructorTest global_constructor_test;

}

[[ gnu::noinline ]]
void test_stack_smash()
{
    printf("running stack smash test...\n");
	char arr[10];
	memset(arr, 0xa9, 12);
}

void test_interrupt_handling()
{
    printf("running interrupt handling test...\n");
    // interrupt 3 is the BREAKPOINT exception, it is modified in the TEST_BUILD to
    // run the interrupt handling test
    __asm__("int $3");
}

void test_global_constructor()
{
    printf("running global constructor test...\n");
    if (global_constructor_test.was_constructed())
        printf("global constructor test: PASSED\n");
    
}

void run_all_tests()
{
    test_global_constructor();
    test_interrupt_handling();
    test_stack_smash();
    printf("\nFINISHED ALL TESTS\n");
    printf("----------\n\n");
}