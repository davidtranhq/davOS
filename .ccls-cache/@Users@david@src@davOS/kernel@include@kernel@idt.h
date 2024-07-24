/**
 * @file idt.h
 * @brief Interface for managing the Interrupt Descriptor Table
 */

#ifndef DAVOS_KERNEL_IDT_H_INCLUDED
#define DAVOS_KERNEL_IDT_H_INCLUDED

/**
 * @brief Initialize the IDT, populating the entries and loading its address into the IDTR.
 */
void idt_init();

#endif