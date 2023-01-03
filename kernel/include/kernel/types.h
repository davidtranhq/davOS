#ifndef DAVOS_KERNEL_TYPES_H_INCLUDED
#define DAVOS_KERNEL_TYPES_H_INCLUDED

/**
 * @brief A symbol defined in a linker script that refers to a address.
 * 
 * Declaring this as a char[] makes it an incomplete type, which prevents the catastrophic
 * mistake of accidentally reading the garbage stored at the address. It can only be used
 * as a pointer type.
 */
using LinkerAddress = char[];

#endif