#ifndef DAVOS_KERNEL_IDT_TYPE_H_INCLUDED
#define DAVOS_KERNEL_IDT_TYPE_H_INCLUDED

#include <stdint.h>

#include <frg/array.hpp>
#include <kernel/SegmentSelector.h>

class IDT
{
public:
    static constexpr uint16_t num_gates = 256;
    // size of each gate in bytes
    static constexpr uint8_t gate_size = 16;
    // size of the entire internal byte representation of the IDT
    static constexpr uint16_t size = gate_size * num_gates;

    enum class GateType
    {
        interrupt = 0xe,
        trap = 0xf
    };

    using InterruptFunction = void (*)(void *interrupt_frame);

public:
    /**
     * @brief Load the bytes of a gate into the IDT at the specified index.
     * 
     * @param index the index into the IDT [0..num_gates)
     * @param interrupt_service_routine the interrupt function to call
     * @param selector a segment selector pointing to a valid code segment in the GDT
     * @param interrupt_stack_table_offset indicates which stack to use for this interrupt
     * @param gate_type specifies if the gate is an interrupt or a trap gate
     * @param required_privilege required CPU privilege for this gate: hardware interrupts ignore this
     */
    void load_gate_descriptor(
        uint16_t index,
        InterruptFunction interrupt_service_routine,
        SegmentSelector selector,
        uint8_t interrupt_stack_table_offset,
        GateType gate_type,
        PrivilegeLevel required_privilege
    );

    /**
     * @brief Get a pointer to the start of the internal byte representation of the IDT.
     */
    const uint8_t *address() const;

private:
    frg::array<uint8_t, size> byte_representation_ {};
};

#endif