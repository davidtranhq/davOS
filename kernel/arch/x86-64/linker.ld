/* Tell the linker that we want an x86_64 ELF64 output file */
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
 
/* The entry point of the operating system */
ENTRY(kernel_pre_main)
 
kernel_stack_size = 64K;

/* Define the program headers we want so the bootloader gives us the right */
/* MMU permissions */
PHDRS
{
    text     PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata   PT_LOAD    FLAGS((1 << 2)) ;            /* Read only */
    data     PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
    reserved PT_LOAD    FLAGS(0) ;                   /* No perimssions */
}
 
SECTIONS
{
    /* We wanna be placed in the topmost 2GiB of the address space, for optimisations */
    /* and because that is what the Limine spec mandates. */
    /* Any address in this region will do, but often 0xffffffff80000000 is chosen as */
    /* that is the beginning of the region. */
    . = 0xffffffff80000000;

    kernel_readonly_start = .;
 
    .text : {
        *(.text .text.*)
    } :text
 
    /* Move to the next memory page for .rodata */
    . += CONSTANT(MAXPAGESIZE);
 
    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    /* Expose a reference to the init_array, containing pointers to the constructors
    for every global object */
    .init_array : {
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP (*(.init_array .ctors))
		PROVIDE_HIDDEN (__init_array_end = .);
	} :rodata

    .stack_guard ALIGN(CONSTANT(MAXPAGESIZE)): {
        . += CONSTANT(MAXPAGESIZE);
    } :reserved

    kernel_readonly_end = .;
    kernel_rw_start = .;
    
    .stack ALIGN(CONSTANT(MAXPAGESIZE)): {
        . += kernel_stack_size;
    } :data

    kernel_stack_start = .;

    /* .data is on a different page than the stack since the stack is 8-byte aligned */
    .data : {
        *(.data .data.*)
    } :data
 
    .bss : {
        *(COMMON)
        *(.bss .bss.*)
    } :data
 
    kernel_rw_end = .;

    /* Discard .note.* and .eh_frame since they may cause issues on some hosts. */
    /DISCARD/ : {
        *(.eh_frame)
        *(.note .note.*)
    }
}
