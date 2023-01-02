#include <stdio.h>

#include <compiler/macros.h>
#include <kernel/kernel.h>
#include <kernel/limine.h>
#include <kernel/terminal.h>
#include <kernel/tests.h>
 
BEGIN_CDECLS

/**
 * @brief The entry point of the operating system.
 */
void kernel_main(void);

END_CDECLS

void kernel_main()
{
    kernel_init();
    
    printf("Hello, world!\n");
    printf("The quick brown fox jumped over the lazy dog!?");
    
    kernel_hang();
}