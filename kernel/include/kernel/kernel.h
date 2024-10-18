#pragma once

#include <stdarg.h>

/**
 * @brief Initialize kernel services such as the terminal, IDT, etc.
 */
void kernel_init();

/**
 * @brief Induce a kernel panic, printing an error message to the terminal.
 * 
 * @param fmt A format-specifier for the string
 * @param ... Variadic arguments for the format specifier
 */
[[ noreturn ]]
void kernel_panic(const char *fmt, ...);

/**
 * @brief Hang the kernel indefinitely.
 */
[[ noreturn ]]
void kernel_hang();

/**
 * @brief Assert that the given condition is true, otherwise panic.
 * 
 * @param condition The condition to assert
 * @param fmt A format-specifier for the string
 * @param ... Variadic arguments for the format specifier
 */
inline void kernel_assert(bool condition, const char *fmt, ...)
{
	if (!condition) {
		va_list args;
		va_start(args, fmt);
		kernel_panic(fmt, args);
		va_end(args);
	}
}
