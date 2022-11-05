#include <stdio.h>

extern "C"
{
#include <kernel/tty.h>

void kernel_main(void);
}

struct GlobalConstructorTest
{
	GlobalConstructorTest() : c {'X'} {}
	char c;
};

GlobalConstructorTest g;

void kernel_main(void) {
	terminal_initialize();
	printf("Hello, kernel!\n");

	printf("%c", g.c);
}
