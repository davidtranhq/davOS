#ifndef DAVOS_KERNEL_PANIC_H_INCLUDED
#define DAVOS_KERNEL_PANIC_H_INCLUDED

#include <compiler/macros.h>

BEGIN_CDECLS

/**
 * @brief Induce a kernel panic, printing an error message to the terminal.
 * 
 * @param fmt A format-specifier for the string
 * @param ... Variadic arguments for the format specifier
 */
__attribute__((__noreturn__)) 
void kernel_panic(const char *fmt, ...);

/**
 * @brief Hang the kernel indefinitely.
 */
__attribute__((__noreturn__))
void kernel_hang();

END_CDECLS

#endif
