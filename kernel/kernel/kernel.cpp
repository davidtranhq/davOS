#include <stdarg.h>
#include <stdio.h>

#include <kernel/frame_allocator.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/kernel.h>
#include <kernel/Terminal.hpp>
#include <kernel/types.h>
#include <kernel/paging.h>
#include <kernel/processor.hpp>
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
    KernelTerminal::initialize();
    gdt_init();
    idt_init();
    frame_allocator_init();
    paging_init();
    vmm_init();
    processor::localAPIC.enableAPIC();
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
