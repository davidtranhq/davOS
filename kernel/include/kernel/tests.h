#ifndef DAVOS_KERNEL_TESTS_H_INCLUDED
#define DAVOS_KERNEL_TESTS_H_INCLUDED

void run_all_tests();

void test_stack_smash();

void test_interrupt_handling();

void test_global_constructor();

void test_stack_overflow();

void test_paging();

#endif