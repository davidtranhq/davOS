#include <stdio.h>
#include <string.h>

#include <compiler/macros.h>
#include <kernel/kernel.h>
#include <kernel/limine.h>
#include <kernel/terminal.h>
 
BEGIN_CDECLS

void kernel_main(void);

END_CDECLS

struct GlobalConstructorTest
{
    GlobalConstructorTest()
    {
        printf("I was constructed!\n");
    }
};

[[ gnu::noinline ]]
void stack_smash_test()
{
	char arr[10];
	memset(arr, 0xa9, 12);
}

void kernel_main()
{
    terminal_init();
    printf("Hello, world!\n");
    GlobalConstructorTest g;
    stack_smash_test();

    kernel_hang();
}