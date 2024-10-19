#pragma once

/**
 * @brief The local APIC is an interrupt controller responsible for managing and handling interrupts
 * for a specific processor. It is responsible for inter-processor interrupts (IPIs), timer interrupts,
 * and forwarding other hardware interrupts (e.g., from an I/O APIC) to the processor.
 */
class LocalAPIC {
    struct Registers;
public:
    /**
     * @brief Enable and configure the local APIC, setting and mapping the physical base address,
     * remapping the PIC to avoid conflicts, and disabling the PIC by masking all PIC interrupts.
     *
     * The spurious interrupt vector is mapped to 0xff, and the PIC1 and PIC2 interrupts are
     * mapped to start at 0x20 and 0x28 respectively. (0x20 to 0x27, and 0x28 to 0x2f, respectively).
     */
    void enableAPIC();

private:
    Registers* m_baseAddress = nullptr;
};
