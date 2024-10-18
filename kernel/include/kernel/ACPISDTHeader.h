#pragma once

#include <cstdint>

struct ACPISDTHeader {
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
        std::size_t byteSum = 0;
        for (std::size_t i = 0; i < length; ++i)
            byteSum += reinterpret_cast<const char *>(this)[i];
        return (byteSum % 0x100) == 0;
    }
};
