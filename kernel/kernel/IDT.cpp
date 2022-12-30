#include <kernel/IDT.h>

void IDT::load_gate_descriptor(
    uint16_t index,
    InterruptFunction interrupt_service_routine,
    SegmentSelector selector,
    uint8_t interrupt_stack_table_offset,
    GateType gate_type,
    PrivilegeLevel required_privilege
)
{
    /**
     * descriptor byte representation:
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
    uint16_t base = index * IDT::gate_size;

    uint16_t selector_bytes = selector.to_uint16_t();
    uint64_t isr_address = reinterpret_cast<uint64_t>(interrupt_service_routine);

    byte_representation_[base + 0] = isr_address;
    byte_representation_[base + 1] = isr_address >> 8;
    byte_representation_[base + 2] = selector_bytes;
    byte_representation_[base + 3] = selector_bytes >> 8;
    byte_representation_[base + 4] = interrupt_stack_table_offset & 7;
    byte_representation_[base + 5] = static_cast<uint8_t>(gate_type)
                                   | static_cast<uint8_t>(required_privilege) << 5
                                   | 1 << 7; // present bit 47: must be 1 for the gate to be valid
    byte_representation_[base + 6] = isr_address >> 16;
    byte_representation_[base + 7] = isr_address >> 24;
    byte_representation_[base + 8] = isr_address >> 32;
    byte_representation_[base + 9] = isr_address >> 40;
    byte_representation_[base + 10] = isr_address >> 48;
    byte_representation_[base + 11] = isr_address >> 56;
    // bytes 12-15 are undefined by the x86_64 standard

    // TODO: remove debug
    byte_representation_[base + 12] = 0xca;
    byte_representation_[base + 13] = 0xfe;
    byte_representation_[base + 14] = 0xba;
    byte_representation_[base + 15] = 0xbe;
}

const uint8_t *IDT::address() const
{
    return byte_representation_.data();
}