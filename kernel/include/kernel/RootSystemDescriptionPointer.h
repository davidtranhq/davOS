#ifndef DAVOS_KERNEL_ACPI_H
#define DAVOS_KERNEL_ACPI_H

#include <kernel/kernel.h>

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

    uint64_t versionOnePropertySum() const
    {
        uint64_t sum = partialChecksum + acpiRevision + rsdtPhysicalAddress;
        for (int i = 0; i < signatureLength; ++i)
            sum += signature[i];
        for (int i = 0; i < oemIdLength; ++i)
            sum += oemId[i];
        return sum;
    }

    uint64_t versionTwoPropertySum() const
    {
        return rsdpSize + xsdtPhysicalAddress + fullChecksum + reserved[0] + reserved[1] + reserved[2];
    }

    bool verifyPartialChecksum() const
    {
        return !(versionOnePropertySum() & 0xff);
    }

    bool verifyFullChecksum() const
    {
        return !((versionOnePropertySum() + versionTwoPropertySum()) & 0xff);
    }

    bool validate() const
    {
        if (acpiRevision == 0)
            return verifyPartialChecksum();
        return verifyFullChecksum();
    }

    uint64_t rootSystemDescriptionTablePointer() const
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

    char signature[8];
    uint8_t partialChecksum;
    char oemId[6];
    uint8_t acpiRevision;
    uint32_t rsdtPhysicalAddress;
    uint32_t rsdpSize;
    uint64_t xsdtPhysicalAddress;
    uint8_t fullChecksum;
    uint8_t reserved[3];
} __attribute__ ((packed));

#endif
