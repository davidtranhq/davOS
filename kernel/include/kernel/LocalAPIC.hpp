#pragma once

/**
 * @brief The local APIC is an interrupt controller responsible for managing and handling interrupts
 * for a specific processor. It is responsible for inter-processor interrupts (IPIs), timer interrupts,
 * and forwarding other hardware interrupts (e.g., from an I/O APIC) to the processor.
 */
class LocalAPIC {
public:
    /**
     * @brief Enable and configure the local APIC, setting and mapping the physical base address,
     * remapping the PIC to avoid conflicts, and disabling the PIC by masking all PIC interrupts.
     *
     * The spurious interrupt vector is mapped to 0xff, and the PIC1 and PIC2 interrupts are
     * mapped to start at 0x20 and 0x28 respectively. (0x20 to 0x27, and 0x28 to 0x2f, respectively).
     */
    void enableAPIC();

    /**
     * @brief Send the End of Interrupt (EOI) signal to the local APIC. This needs to be called
     * after handling an interrupt to inform the local APIC that the interrupt has been processed
     * to allow it to handle further interrupts.
     */
    void sendEndOfInterrupt();

    void write(uint32_t regOffset, uint32_t value);
    uint32_t read(uint32_t regOffset);

    volatile uint32_t *baseAddress = nullptr;
};
