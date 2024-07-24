#ifndef DAVOS_KERNEL_TABLE_DESCRIPTOR_H_INCLUDED
#define DAVOS_KERNEL_TABLE_DESCRIPTOR_H_INCLUDED

#include <stdint.h>
#include <dav/array.hpp>

/**
 * @brief A data structure used to contain information about
 * the Global Descriptor Table and the Interrupt Descriptor Table.
 * 
 * Its address is used as an argument to the GIDT and LIDT instructions, reps
 */
class TableDescriptor
{
public:
    // 2 bytes for the IDT size, 8 bytes for the IDT address
    static constexpr uint8_t size = 2 + 8;

public:
    /**
     * @brief Initializes the table descriptor structure.
     * 
     * @param size the size of the table in bytes 
     * @param table_address a pointer to the internal byte representation of the 
     * table.
     */
    TableDescriptor(uint16_t size, const void *table_address);

    /**
     * @brief Get a pointer to the internal byte representation.
     */
    const uint8_t *address() const;

private:
    dav::Array<uint8_t, size> byte_representation_;
};

#endif
