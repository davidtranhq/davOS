#pragma once

#include <cstdint>
#include <kpp/Array.hpp>
#include <kernel/ACPISDTHeader.h>

/**
 * @brief The Multiple APIC Description Table (MADT) is a table in the ACPI
 *       specification that describes the interrupt controller structure of the
 *       system.
 */
struct MADT {
    ACPISDTHeader header;
    uint32_t localApicAddress;
    uint32_t flags;
    void* apicStructures;
} __attribute__((packed));

class APICManager {
    struct LocalAPICInfo {
        uint8_t processorId;
        uint8_t apicId;
        uint32_t flags;
    } __attribute__((packed));

    struct IOAPICInfo {
        uint8_t ioApicId;
        uint8_t reserved;
        uint32_t ioApicAddress;
        uint32_t globalSystemInterruptBase;
    } __attribute__((packed));

    struct IOAPICInterruptSourceOverride {
        uint8_t busSource;
        uint8_t irqSource;
        uint32_t globalSystemInterrupt;
        uint16_t flags;
    } __attribute__((packed));

    class IOAPIC {
    public:
        IOAPIC() = default;
        IOAPIC(uint32_t address, uint32_t globalSystemInterruptBase, uint8_t id);

        void writeRegister(uint8_t reg, uint32_t value) const;
        uint32_t readRegister(uint8_t reg) const;
        inline uint8_t id() const { return m_id; }
        inline bool handlesIRQ(uint8_t irq) const { return irq >= m_globalSystemInterruptBase && irq < m_globalSystemInterruptBase + m_numRedirectionEntries; }
        inline uint32_t readRedirectionEntryLow(uint8_t irq) const
        { 
            return readRegister(0x10 + 2 * (irq - m_globalSystemInterruptBase));
        }
        inline uint32_t readRedirectionEntryHigh(uint8_t irq) const
        {
            return readRegister(0x10 + 2 * (irq - m_globalSystemInterruptBase) + 1);
        }
        inline void writeRedirectionEntryLow(uint8_t irq, uint32_t value) const
        {
            writeRegister(0x10 + 2 * (irq - m_globalSystemInterruptBase), value);
        }
        inline void writeRedirectionEntryHigh(uint8_t irq, uint32_t value) const
        {
            writeRegister(0x10 + 2 * (irq - m_globalSystemInterruptBase) + 1, value);
        }

    private:
        uint32_t m_address {};
        uint32_t m_globalSystemInterruptBase {};
        uint8_t m_id {};
        uint8_t m_numRedirectionEntries {};
    };

public:
    /**
     * @brief Initialize the APIC manager by parsing the RSDT pointed to by the RSDP
     * and any other necessary tables.
     */
    void initialize();

    /**
     * @brief Redirect a given IRQ to a the local APIC on this processor.
     */
    void redirectIrq(uint8_t irq, uint8_t vector);

    /**
     * @brief Send an interrupt to the processor with the given APIC ID.
     */
    void sendInterprocessorInterrupt(uint8_t apicId, uint8_t vector);

private:
    struct RootSystemDescriptionPointer* rsdp;
    kpp::Array<IOAPIC, 16> m_ioApics;
    std::size_t m_numLocalApics {0};
    std::size_t m_numIoApics {0};
};
