#include <stdio.h>

#include <compiler/macros.h>
#include <kernel/kernel.h>
#include <kernel/limine.h>
#include <kernel/terminal.h>
#include <kernel/tests.h>
 
BEGIN_CDECLS

void kernel_main(void);

END_CDECLS

void kernel_main()
{
    terminal_init();
    printf("Hello, world!\n");
    test_stack_smash();
    
    kernel_hang();
}