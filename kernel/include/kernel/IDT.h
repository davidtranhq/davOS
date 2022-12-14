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

    /**
     * @brief Contains the information required for one entry in the IDT.
     */
    struct GateDescriptor
    {
        // a function pointer to the ISR callback void isr(*interrupt_frame)
        IDT::InterruptFunction interrupt_service_routine;
        // a segment selector pointing to a valid code segment in the GDT
        SegmentSelector selector;
        // specifies which stack to use (stored in the interrupt stack table) for the ISR
        uint8_t interrupt_stack_table_offset;
        // differentiates between interrupts and traps
        IDT::GateType gate_type;
        // required CPU privilege for this gate: hardware interrupts ignroe this
        PrivilegeLevel required_privilege;
    };


public:
    /**
     * @brief Load the bytes of a gate into the IDT at the specified index.
     * 
     * @param index the index into the IDT [0..num_gates)
     * @param gate_descriptor information about the gate descriptor
     */
    void load_gate_descriptor(uint16_t index, const GateDescriptor &gate_descriptor);

    /**
     * @brief Get a pointer to the start of the internal byte representation of the IDT.
     */
    const uint8_t *address() const;

private:
    frg::array<uint8_t, size> byte_representation_ {};
};

#endif