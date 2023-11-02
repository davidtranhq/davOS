#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/frame_allocator.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/kernel.h>
#include <kernel/terminal.h>
#include <kernel/vmm.h>

extern "C" void (*__init_array_start)(), (*__init_array_end)();

namespace
{

void call_global_constructors()
{
    for (auto ctor = &__init_array_start; ctor < &__init_array_end; ctor++)
    {
        (*ctor)();
    }
}

} // anonymous namespace

void kernel_init()
{
    call_global_constructors();
    gdt_init();
    idt_init();
    terminal_init();
    frame_allocator_init();
    vmm_init();
}

[[ noreturn ]]
void kernel_panic(const char *fmt, ...)
{
    printf("KERNEL PANIC: ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    kernel_hang();
}

[[ noreturn ]]
void kernel_hang()
{
    for (;;)
        __asm__("hlt");
}
