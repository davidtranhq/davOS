/**
 * @file gdt.h
 * @brief Interface for initializing the Global Decsriptor Table
 */

#ifndef DAVOS_KERNEL_GDT_H_INCLUDED
#define DAVOS_KERNEL_GDT_H_INCLUDED

/**
 * @brief The segments defined in the global descriptor table.
 * 'code' is readable and 'data' is writable.
 */
enum class GDTSegment
{
    null = 0,
    kernel_code = 1,
    kernel_data = 2,
    user_code = 3,
    user_data = 4
};

/**
 * @brief Initialize the GDT. The entries are populated with the
 * following segments in order:
 * 
 * Null descriptor
 * 16-bit code descriptor. Base = 0, limit = 0xffff. Readable.
 * 16-bit data descriptor. Base = 0, limit = 0xffff. Writable.
 * 32-bit code descriptor. Base = 0, limit = 0xffffffff. Readable.
 * 32-bit data descriptor. Base = 0, limit = 0xffffffff. Writable.
 * 64-bit code descriptor. Base and limit irrelevant. Readable.
 * 64-bit data descriptor. Base and limit irrelevant. Writable.
 */
void gdt_init();

#endif