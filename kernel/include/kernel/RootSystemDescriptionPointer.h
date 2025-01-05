#pragma once

#include <cstdint>
#include <kernel/kernel.h>
#include <kernel/limine_features.h>

/*
 * A data structure used for the ACPI programming interface.
 *
 * The data layout for a data structure in memory containing the address of the root system
 * description table (RSDT) and the extended system descriptor table (XSDT).
 *
 * Note this struct isn't meant to be constructed, but only used to prescribe the manipulate
 * the bytes in memory that were initialized by the bootloader. E.g.
 *
 * void* ptr = initializedByBootLoader();
 * RootSystemDescriptionPointer *rsdp = reinterpret_cast<RootSystemDescriptionPointer *>(ptr);
 *
 * https://wiki.osdev.org/RSDP
 */
struct RootSystemDescriptionPointer {
    static constexpr int signatureLength = 8;
    static constexpr int oemIdLength = 6;

    bool verifySignature() const
    {
        const char *expectedSignature = "RSD PTR ";
        for (int i = 0; i < signatureLength; ++i) {
            if (signature[i] != expectedSignature[i])
                return false;
        }
        return true;
    }

    bool verifyPartialChecksum() const
    {
        uint8_t sum {0};
        auto firstByte = reinterpret_cast<const uint8_t*>(this);
        auto lastByte = reinterpret_cast<const uint8_t*>(&rsdpSize);
        for (auto byte = firstByte; byte < lastByte; ++byte)
            sum += *byte;
        return !sum;
    }

    bool verifyFullChecksum() const
    {
        uint8_t sum {0};
        auto firstByte = reinterpret_cast<const uint8_t*>(this);
        auto lastByte = firstByte + rsdpSize;
        for (auto byte = firstByte; byte < lastByte; ++byte)
            sum += *byte;
        return !sum;
    }

    bool validate() const
    {
        if (acpiRevision == 0)
            return verifyPartialChecksum();
        return verifyFullChecksum();
    }

    uint64_t rootSDTPhysicalAddress() const
    {
        /*
         * It's likely that the RSDT and XSDT pointers point to the same location for compatibility reasons,
         * but the ACPI standards specify that ACPI version 2.0 uses the XSDT address, so we comply
         * with the standards here.
         */
        if (!validate())
            kernel_panic("invalid RSDP table\n");
        return acpiRevision == 0 ? rsdtPhysicalAddress : xsdtPhysicalAddress;
    }

    uint8_t signature[8];
    uint8_t partialChecksum;
    uint8_t oemId[6];
    uint8_t acpiRevision;
    uint32_t rsdtPhysicalAddress;
    uint32_t rsdpSize;
    uint64_t xsdtPhysicalAddress;
    uint8_t fullChecksum;
    uint8_t reserved[3];
} __attribute__ ((packed));

