#include <stdio.h>

#include <compiler/macros.h>
#include <kernel/kernel.h>
#include <kernel/limine.h>
#include <kernel/terminal.h>
#include <kernel/tests.h>
#include <kernel/types.h> // Symbol

int stack_overflow(int x)
{
    printf("%x ", x);
    return stack_overflow(x + 1);
}

[[ noreturn ]]
void kernel_main()
{
    kernel_init();
    
    printf("Hello, world!\n");

    kernel_hang();
}

extern LinkerAddress stack_start;
extern "C"
[[ noreturn ]]
void kernel_pre_main()
{
    // Replace Limine's stack with our own so that we can reclaim bootloader-reclaimable memory.
    // It's fine that we throw away the old stack contents since there are no stack variables
    // in this function and we don't plan on returning to it (kernel_main() is noreturn)
    __asm__("mov %%rsp, %0" :: "r"(stack_start));

    kernel_main();
}