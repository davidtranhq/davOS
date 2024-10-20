#pragma once

#include <cstdint>
#include <dav/Array.hpp>
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
};

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

public:
    /**
     * @brief Initialize the APIC manager by parsing the RSDT pointed to by the RSDP
     * and any other necessary tables.
     */
    void initialize();

private:

    dav::Array<LocalAPICInfo, 64> m_localApicInfos;
    dav::Array<IOAPICInfo, 16> m_ioApicInfos;
    std::size_t m_numLocalApics;
    std::size_t m_numIoApics;
};
