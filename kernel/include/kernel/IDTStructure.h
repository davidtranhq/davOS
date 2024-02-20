#ifndef DAVOS_KERNEL_IDT_STRUCTURE_H_INCLUDED
#define DAVOS_KERNEL_IDT_STRUCTURE_H_INCLUDED

#include <cstdint>

#include <dav/array.hpp>
#include <kernel/SegmentSelector.h>

class IDTStructure
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

    struct InterruptFrame
    {
        uint64_t ip;
        uint64_t cs;
        uint64_t flags;
        uint64_t sp;
        uint64_t ss;
    } __attribute__((packed));

    using InterruptFunction = union
    {
        void (*no_error_code)(InterruptFrame *);
        void (*with_error_code)(InterruptFrame *, uint64_t);
    };

    /**
     * @brief Contains the information required for one entry in the IDT.
     */
    struct GateDescriptor
    {
        // a function pointer to the ISR callback void isr(*interrupt_frame)
        IDTStructure::InterruptFunction interrupt_service_routine;
        // a segment selector pointing to a valid code segment in the GDT
        SegmentSelector selector;
        // specifies which stack to use (stored in the interrupt stack table) for the ISR
        uint8_t interrupt_stack_table_offset;
        // differentiates between interrupts and traps
        IDTStructure::GateType gate_type;
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
    dav::array<uint8_t, size> byte_representation_ {};
};

#endif