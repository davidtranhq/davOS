/**
 * @file paging.h
 * @brief Interface for initializing a page table.
 */

#ifndef DAVOS_KERNEL_PAGING_H_INCLUDED
#define DAVOS_KERNEL_PAGING_H_INCLUDED

/**
 * @brief Initialize and load a page directory and its corresponding page tables.
 * 
 * The page directory maps all frames required for the kernel to function,
 * including the kernel itself and other facilities like the stack and the framebuffer for printing.
 */
void paging_init();

#endif