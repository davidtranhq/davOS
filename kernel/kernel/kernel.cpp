#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/kernel.h>
#include <kernel/terminal.h>

[[ noreturn ]]
void kernel_panic(const char *fmt, ...)
{
    printf("KERNEL PANIC: ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    while (1) {}
    ::__builtin_unreachable();
}

[[ noreturn ]]
void kernel_hang()
{
    for (;;)
        __asm__("hlt");
}