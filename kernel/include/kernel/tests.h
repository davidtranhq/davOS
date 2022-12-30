#ifndef DAVOS_KERNEL_TESTS_H_INCLUDED
#define DAVOS_KERNEL_TESTS_H_INCLUDED

struct TestGlobalConstructor
{
    TestGlobalConstructor();
};

void test_stack_smash();

void test_interrupt_divide_by_zero();

#endif