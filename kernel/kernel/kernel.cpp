#include <stdio.h>
#include <string.h>

#include <kernel/panic.h>
#include <kernel/terminal.h>

__attribute__((__noinline__))
void stack_smash_test()
{
	char arr[10];
	memset(arr, 0xa9, 12);
}

extern "C" void kernel_main(void) {
	terminal_init();
	// if the stack is smashed, stack_smash_test() should invoke a panic and not return
	stack_smash_test();
	printf("The stack is safe.\n");
}
