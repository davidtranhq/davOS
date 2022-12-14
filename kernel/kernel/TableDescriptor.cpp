#include <kernel/TableDescriptor.h>
#include <kernel/kernel.h>

TableDescriptor::TableDescriptor(uint16_t size, const void *table_address)
{
    // the size parameter is one less than the actual table size in bytes
    uint16_t adjusted_size = size - 1;
    byte_representation_[0] = adjusted_size;
    byte_representation_[1] = adjusted_size >> 8;

    const uint64_t address = reinterpret_cast<uint64_t>(table_address); 
    byte_representation_[2] = address;
    byte_representation_[3] = address >> 8;
    byte_representation_[4] = address >> 16;
    byte_representation_[5] = address >> 24;
    byte_representation_[6] = address >> 32;
    byte_representation_[7] = address >> 40;
    byte_representation_[8] = address >> 48;
    byte_representation_[9] = address >> 56;
}

const uint8_t *TableDescriptor::address() const
{
    return byte_representation_.data();
}