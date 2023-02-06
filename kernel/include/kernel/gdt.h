/**
 * @file gdt.h
 * @brief Interface for initializing the Global Decsriptor Table
 */

#ifndef DAVOS_KERNEL_GDT_H_INCLUDED
#define DAVOS_KERNEL_GDT_H_INCLUDED

#include <cstdint>

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
 * 64-bit kernel code descriptor. Base and limit irrelevant. Readable.
 * 64-bit kernel data descriptor. Base and limit irrelevant. Writable.
 * 64-bit user code descriptor. Base and limit irrelevant. Readable.
 * 64-bit data code descriptor. Base and limit irrelevant. Writable.
 */
void gdt_init();


/**
 * @brief Reloads the CS, DS, ES, FS, GS, and SS segment registers.
 * 
 * @param gdt_code_segment_offset offset into the GDT pointing to a code segment. Should be a 
 * multiple of 8 (since each segment is 8 bytes)
 * @param gdt_data_segment_offset offset into the GDT pointing to a data segment. Should be a
 * multiple of 8 (since each segment is 8 bytes)
 */
extern "C"
void reload_segment_registers(uint64_t gdt_code_segment_offset, uint64_t gdt_data_segment_offset);

#endif