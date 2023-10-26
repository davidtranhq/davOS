.global load_ptbr

/**
 * arguments:
 *  page table base physical address => %rdi
 */
load_ptbr:
    shlq $12, %rdi  // << 12 the address; since the address in the CR3 register is stored
                    // from bit 12 onwards
    movq %rdi, %cr3 // move the shifted address into the PTBR (CR3 register)
    ret