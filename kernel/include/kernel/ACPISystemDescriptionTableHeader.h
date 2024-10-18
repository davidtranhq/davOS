#ifndef ACPI_SYSTEM_DESCRIPTION_TABLE_HEADER_H
#define ACPI_SYSTEM_DESCRIPTION_TABLE_HEADER_H

struct ACPISystemDescriptionTableHeader {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemId[6];
    char oemTableId[8];
    uint32_t oemRevision;
    uint32_t creatorId;
    uint32_t creatorRevision;

    bool verifyChecksum() const
    {
        size_t byteSum = 0;
        for (size_t i = 0; i < length; ++i)
            byteSum += reinterpret_cast<char *>(this)[i];
        return (byteSum % 0x100) == 0;
    }
};

#endif
