#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/panic.h>
#include <kernel/terminal.h>

[[ noreturn ]]
void panic(const char *fmt, ...)
{
    printf("KERNEL PANIC: ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    while (1) {}
    ::__builtin_unreachable();
}