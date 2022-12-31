#include <stdio.h>

#include <compiler/macros.h>
#include <kernel/kernel.h>
#include <kernel/limine.h>
#include <kernel/terminal.h>
#include <kernel/tests.h>
 
BEGIN_CDECLS

void kernel_main(void);

END_CDECLS

struct limine_memmap_request request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

void kernel_main()
{
    kernel_init();
    
    printf("Hello, world!\n");
    printf("Memory Map:\n");
    for (uint64_t i = 0; i < request.response->entry_count; ++i) {
        auto entry = request.response->entries[i];
        printf("base: %llu, length: %llu, type: %llu\n", entry->base, entry->length, entry->type);
    }

    kernel_hang();
}