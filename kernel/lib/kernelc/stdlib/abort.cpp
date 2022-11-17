#include <stdio.h>

extern "C"
[[ noreturn ]]
void abort(void)
{
	printf("abort()\n");
	__builtin_trap();
	while (1) {}
	__builtin_unreachable();
}

