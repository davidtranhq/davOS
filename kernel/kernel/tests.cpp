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

int test_interrupt_divide_by_zero()
{
    int x = 0;
    return 1 / x;
}