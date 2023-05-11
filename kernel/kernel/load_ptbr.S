.global load_ptbr

/**
 * arguments:
 *  page table base physical address => %rdi
 */
load_ptbr:
    //shl %rdi, 12    // << 12 the address; since the address in the CR3 register is stored
                    // from bit 12 onwards
    mov %cr3, %rdi  // move the shifted address into the PTBR (CR3 register)
    ret