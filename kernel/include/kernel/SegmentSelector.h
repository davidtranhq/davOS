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
 * @brief The segments defined in the global descriptor table.
 * 'code' is readable and 'data' is writable.
 * 'codeXX' means XX-bit segment: e.g. code16 is a writable segment with a limit of 0xffff.
 * 
 * The segments are defined and initialized by the Limine boot protocol.
 * See https://github.com/limine-bootloader/limine/blob/trunk/PROTOCOL.md .
 */
enum class GDTSegment
{
    null = 0,
    code16 = 1,
    data16 = 2,
    code32 = 3,
    data32 = 4,
    code64 = 5,
    data64 = 6
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
        GDTSegment gdt_segment
    ) : requested_privilege_level_ {requested_privilege_level},
        table_type_ {table_type},
        gdt_segment_ {gdt_segment}
    {}


    /**
     * Convert the segment selector to its raw 16-bit binary sequence.
     * 
     * Bits:
     *      0-1: requested privilege level
     *      2: descriptor table type; 0 = GDT, 1 = LDT
     *      3-15: index into the table entry
     */
    constexpr uint16_t to_uint16_t() const
    {
        return static_cast<uint16_t>(gdt_segment_) << 3 
             | static_cast<uint16_t>(table_type_) << 2 
             | static_cast<uint16_t>(requested_privilege_level_);
    }

private:
    PrivilegeLevel requested_privilege_level_;
    DescriptorTable table_type_;
    GDTSegment gdt_segment_;
};

#endif