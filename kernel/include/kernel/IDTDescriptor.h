#ifndef DAVOS_KERNEL_IDT_DESCRIPTOR_H_INCLUDED
#define DAVOS_KERNEL_IDT_DESCRIPTOR_H_INCLUDED

#include <stdint.h>
#include <frg/array.hpp>

class IDTDescriptor
{
public:
    // 2 bytes for the IDT size, 8 bytes for the IDT address
    static constexpr uint8_t size = 2 + 8;

public:
    /**
     * @brief Initializes the IDTDescriptor structure.
     * 
     * @param size the size of the IDT in bytes 
     * @param idt_address a pointer to the internal byte representation of the IDT
     */
    IDTDescriptor(uint16_t size, const uint8_t *idt_address);

    /**
     * @brief Get a pointer to the internal byte representation.
     */
    const uint8_t *address() const;

private:
    frg::array<uint8_t, size> byte_representation_;
};

#endif