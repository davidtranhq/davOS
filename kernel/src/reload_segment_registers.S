.global reload_segment_registers

/**
 * arguments:
 *  GDT code segment offset => %rdi
 *  GDT data segment offset => %rsi
 */
reload_segment_registers:
    // The code segment (CS) register can't be loaded directly:
    // it can only be changed by a far return instruction, which pops a new
    // value for CS off the stack.
    
    // push the new value for CS to be loaded
    // %rdi contains the argument with the offset in the GDT pointing to a kernel code segment
    // i.e. GDT base + 8 => 2nd entry in the GDT (1st entry is the null descriptor)
    // since each entry is 8 bytes
    push %rdi

    // load the address of the routine to return to
    lea reload_data_segment_registers(%rip), %rax

    // push this address to the stack
    push %rax

    // far return: this pops 0x8 into the CS register and returns to
    // reload_other_segment_registers
    retfq

reload_data_segment_registers:
    // %rsi contains the argument with the offset into the GDT for a kernel data segment
    mov %rsi, %rax
    mov %rax, %ds
    mov %rax, %es
    mov %rax, %fs
    mov %rax, %gs
    mov %rax, %ss
    ret
