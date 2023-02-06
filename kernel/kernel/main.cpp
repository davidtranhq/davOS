#include <stdio.h>

#include <kernel/kernel.h>
#include <kernel/limine.h>
#include <kernel/terminal.h>
#include <kernel/tests.h>
#include <kernel/types.h> // LinkerAddress

[[ noreturn ]]
void kernel_main()
{
    kernel_init();

#ifdef TEST_BUILD
    run_all_tests();
#endif
    
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