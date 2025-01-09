#include <kpp/cstdio.hpp>

#include <kernel/APICManager.hpp>
#include <kernel/frame_allocator.h>
#include <kernel/kernel.h>
#include <kernel/limine.h>
#include <kernel/macros.h>
#include <kernel/terminal.h>
#include <kernel/tests.h>
#include <kernel/types.h> // LinkerAddress


extern LinkerAddress kernel_stack_start;

[[ noreturn ]]
void kernel_main()
{
    [[ maybe_unused ]]
    volatile int x = 0;
    kernel_init();

    DEBUG("Stack initialized to %x\n", &kernel_stack_start);

    run_all_tests();
    
    kpp::printf("Hello, world!\n");

    kernel_hang();
}

extern "C"
[[ noreturn ]]
void kernel_pre_main()
{
    // Replace Limine's stack with our own so that we can reclaim bootloader-reclaimable memory.
    // It's fine that we throw away the old stack contents since there are no stack variables
    // in this function and we don't plan on returning to it (kernel_main() is noreturn)
    __asm__("mov %%rsp, %0" :: "r"(&kernel_stack_start));

    kernel_main();
}
