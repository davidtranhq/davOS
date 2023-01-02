#include <kernel/gdt.h>
#include <kernel/TableDescriptor.h>

#include <frg/array.hpp> // std::size

/**
 * @brief A valid segment must be present.
 * 0: not present
 * 1: present
 */
#define PRESENT(b) ((b) << 7)

/**
 * @brief The privilege level required for this segment, either kernel or user.
 */
#define KERNEL_PRIVILEGE 0 << 5
#define USER_PRIVILEGE 3 << 5

/**
 * @brief The type of the segment, either a system segment or a code/data segment.
 */
#define SYSTEM_SEGMENT 0 << 4
#define CODE_OR_DATA_SEGMENT 1 << 4

/**
 * @brief The type of code/data segment, either a code or a data segment. 
 */
#define IS_CODE_SEGMENT(b) ((b) << 3)

/**
 * @brief The direction that a data segment grows
 * or the behaviour of the privilege level of a code segment.
 */
#define DIRECTION_OR_PRIVILEGE(b) ((b) << 2)
#define DATA_GROWS_UPWARD 0 << 2
#define DATA_GROWS_DOWNWARD 1 << 2
#define CODE_EXACT_PRIVILEGE 0 << 2
#define CODE_EQUAL_OR_LOWER_PRIVILEGE 1 << 2

/**
 * @brief The R/W permissions of a data or code segment.
 */
#define DATA_READONLY 0 << 1
#define DATA_WRITABLE 1 << 1
#define CODE_UNREADABLE 0 << 1
#define CODE_READABLE 1 << 1

/**
 * @brief Set if the CPU has accessed this segment.
 * This should initially be cleared.
 */
#define ACCESSED(b) (b)

/**
 * @brief The scaling factor for the 'limit' field in the segment.
 * Byte granularity means that limit is expressed in bytes,
 * page granularity means that limit is expressed in 4 KiB pages.
 */
#define BYTE_GRANULARITY 0 << 3
#define PAGE_GRANULARITY 1 << 3

/**
 * @brief The bit-width of the segment, either 16-bit protected mode or 32-bit protected mode. 
 * 
 */
#define PROTECTED_16 0 << 2
#define PROTECTED_32 1 << 2

/**
 * @brief Set if the segment is a 64-bit code segment.
 * If this is set, BIT_WIDTH should also be LONG_MODE.
 */
#define LONG_MODE_CODE(b) ((b) << 1)

/**
 * @brief Make a segment descriptor for the GDT.
 * 
 * @param base the base address of the segment
 * @param limit the maximum address of the segment
 * @param access access flags for this segment
 * @param flags miscellanous flags (granularity, bit width, long mode code) 
 */
constexpr uint64_t make_segment_descriptor(
    uint64_t base,
    uint64_t limit,
    uint8_t access,
    uint8_t flags
)
{
    /**
     * 0-15: limit
     * 16-39: base
     * 40-47: access byte
     * 48-51: limit
     * 52-55: flags
     * 56-63: base
     */
    return (limit & 0xffff)
        | (base & 0xffffff) << 16
        | static_cast<uint64_t>(access) << 40
        | (limit >> 16) << 48
        | static_cast<uint64_t>(flags & 0xf) << 52
        | (base >> 24) << 56;
}

/**
 * @brief This is required to be the first entry in the GDT.
 */
#define NULL_DESCRIPTOR 0

#define KERNEL_CODE_ACCESS PRESENT(1) \
                         | KERNEL_PRIVILEGE \
                         | CODE_OR_DATA_SEGMENT \
                         | IS_CODE_SEGMENT(1) \
                         | CODE_EXACT_PRIVILEGE \
                         | CODE_READABLE \
                         | ACCESSED(0)

#define KERNEL_DATA_ACCESS PRESENT(1) \
                         | KERNEL_PRIVILEGE \
                         | CODE_OR_DATA_SEGMENT \
                         | IS_CODE_SEGMENT(0) \
                         | DATA_GROWS_UPWARD \
                         | DATA_WRITABLE \
                         | ACCESSED(0)

#define USER_CODE_ACCESS PRESENT(1) \
                       | USER_PRIVILEGE \
                       | CODE_OR_DATA_SEGMENT \
                       | IS_CODE_SEGMENT(1) \
                       | CODE_EXACT_PRIVILEGE \
                       | CODE_READABLE \
                       | ACCESSED(0)

#define USER_DATA_ACCESS PRESENT(1) \
                       | USER_PRIVILEGE \
                       | CODE_OR_DATA_SEGMENT \
                       | IS_CODE_SEGMENT(0) \
                       | DATA_GROWS_UPWARD \
                       | DATA_WRITABLE \
                       | ACCESSED(0)

#define LONG_MODE_CODE_PAGING PAGE_GRANULARITY | LONG_MODE_CODE(1)
#define PROTECTED_32_PAGING PAGE_GRANULARITY | PROTECTED_32


constexpr uint64_t global_descriptor_table[] =
{
    NULL_DESCRIPTOR,
    make_segment_descriptor(0, 0x000fffff, KERNEL_CODE_ACCESS, LONG_MODE_CODE_PAGING),
    make_segment_descriptor(0, 0x000fffff, KERNEL_DATA_ACCESS, PROTECTED_32_PAGING),
    make_segment_descriptor(0, 0x000fffff, USER_CODE_ACCESS, LONG_MODE_CODE_PAGING),
    make_segment_descriptor(0, 0x000fffff, USER_DATA_ACCESS, PROTECTED_32_PAGING)
};

const TableDescriptor gdt_descriptor(
    sizeof(global_descriptor_table),
    global_descriptor_table
);

void gdt_init()
{
    __asm__("lgdt %0" :: "m"(*gdt_descriptor.address()));
}