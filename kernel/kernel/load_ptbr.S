.global load_ptbr

/**
 * arguments:
 *  page table base physical address => %rdi
 */
load_ptbr:
    movq %rdi, %cr3 // move the shifted address into the PTBR (CR3 register)
    ret