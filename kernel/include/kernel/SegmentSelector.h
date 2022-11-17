#ifndef DAVOS_KERNEL_SEGMENT_SELECTOR_H_INCLUDED
#define DAVOS_KERNEL_SEGMENT_SELECTOR_H_INCLUDED

#include <stdint.h>

// Integral representation corresponds to ring levels (e.g. kernel = ring 0)
enum class PrivilegeLevel
{
    kernel = 0,
    user = 3
};

// Integral representation: GDT = 0, LDT = 1
enum class DescriptorTable
{
    global = 0,
    local = 1
};

/**
 * A binary data structure identifying a segment in the Global Descriptor Table
 * or a Local Descriptor Table.
 */
class SegmentSelector
{
public:
    /**
     * Note: The table entry index should be 8-byte aligned (lower 3 bits = 0)
     * (since segment descriptors are 8 bytes in length)
     */
    constexpr SegmentSelector(
        PrivilegeLevel requested_privilege_level,
        DescriptorTable table_type,
        uint16_t table_entry_index
    ) : requested_privilege_level {requested_privilege_level},
        table_type {table_type},
        table_entry_index {table_entry_index}
    {}


    /**
     * Convert the segment selector its raw 16-bit binary sequence.
     * 
     * Bits:
     *      0-1: requested privilege level
     *      2: descriptor table type; 0 = GDT, 1 = LDT
     *      3-15: index into the table entry
     */
    constexpr uint16_t to_uint16_t() const
    {
        return table_entry_index << 3 
             | static_cast<uint16_t>(table_type) << 2 
             | static_cast<uint16_t>(requested_privilege_level);
    }

private:
    PrivilegeLevel requested_privilege_level;
    DescriptorTable table_type;
    uint16_t table_entry_index;
};

#endif