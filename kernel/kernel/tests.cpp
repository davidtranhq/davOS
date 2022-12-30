#include <stdio.h>
#include <string.h>

#include <kernel/tests.h>

TestGlobalConstructor::TestGlobalConstructor()
{
    printf("I was constructed!\n");
}

[[ gnu::noinline ]]
void test_stack_smash()
{
	char arr[10];
	memset(arr, 0xa9, 12);
}

void test_interrupt_divide_by_zero()
{
    __asm__("int $0");
}