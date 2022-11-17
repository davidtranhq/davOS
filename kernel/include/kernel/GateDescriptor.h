#ifndef DAVOS_KERNEL_GATE_DESCRIPTOR_H
#define DAVOS_KERNEL_GATE_DESCRIPTOR_H

#include <stdint.h>

#include <kernel/SegmentSelector.h>
#include <frg/array.hpp>

/**
 * A single entry in the IDT.
 */
class GateDescriptor
{
public:
    enum class GateType
    {
        interrupt = 0xe,
        trap = 0xf
    };

    constexpr GateDescriptor(
        uint64_t interrupt_service_routine_address,
        SegmentSelector selector,
        uint8_t interrupt_stack_table_offset,
        GateType gate_type,
        PrivilegeLevel required_privilege
    ) : interrupt_service_routine_address_ {interrupt_service_routine_address},
        selector_ {selector},
        interrupt_stack_table_offset_ {interrupt_stack_table_offset},
        gate_type_ {gate_type},
        required_privilege_ {required_privilege}
    {}

    /**
     * Convert the gate descriptor to its raw 16-byte byte sequence.
     * 
     * Bits:
     *  0-15: lowest 16 bits of the ISR address
     *  16-31: segment selector
     *  32-34: IST offset
     *  35-39: reserved (undefined)
     *  40-43: gate type
     *  44: 0
     *  45-46: required privilege level to run the ISR specified in this gate
     *  47: present bit; must be 1 for a valid gate
     *  48-63: bits 16-31 of the ISR address
     *  64-95: bits 32-63 of the ISR address
     *  96-127: reserved (undefined)
     */
    constexpr frg::array<uint8_t, 16> to_bytes() const
    {
        frg::array<uint8_t, 16> bytes = {};

        uint16_t selector_bytes = selector_.to_uint16_t();

        bytes[0] = interrupt_service_routine_address_;
        bytes[1] = interrupt_service_routine_address_ >> 8;
        bytes[2] = selector_bytes;
        bytes[3] = selector_bytes >> 8;
        bytes[4] = interrupt_stack_table_offset_ & 7;
        bytes[5] = 1 << 7 // present bit 47: must be 1 for the gate to be valid
                 | static_cast<uint8_t>(required_privilege_) << 5
                 | static_cast<uint8_t>(gate_type_);
        bytes[6] = interrupt_service_routine_address_ >> 16;
        bytes[7] = interrupt_service_routine_address_ >> 16;
        // bytes 8-15 are undefined by the x86_64 standard

        return bytes;
    }

private:
    uint64_t interrupt_service_routine_address_;
    SegmentSelector selector_;
    uint8_t interrupt_stack_table_offset_;
    GateType gate_type_;
    PrivilegeLevel required_privilege_;
};

#endif