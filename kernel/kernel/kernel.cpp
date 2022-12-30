#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/idt.h>
#include <kernel/kernel.h>
#include <kernel/terminal.h>

extern "C" void (*__init_array_start)(), (*__init_array_end)();

void call_global_constructors()
{
    for (auto ctor = &__init_array_start; ctor < &__init_array_end; ctor++)
    {
        (*ctor)();
    }
}

void kernel_init()
{
    call_global_constructors();
    terminal_init();
    idt_init();
}

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