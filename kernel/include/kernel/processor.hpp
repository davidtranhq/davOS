#pragma once

#include <cstdint>
#include <kernel/LocalApic.hpp>

/**
 * @brief Processor-specific data structures.
 */
namespace processor {
inline LocalAPIC localAPIC;
}; // namespace processor


/**
 * Processor-specific functions.
 */
namespace processor {

/**
 * @brief Bitmasks for feature support that can be checked with the CPUID instruction.
 */
enum class CpuIdFeature {
    ECX_SSE3         = 1 << 0,
    ECX_PCLMUL       = 1 << 1,
    ECX_DTES64       = 1 << 2,
    ECX_MONITOR      = 1 << 3,
    ECX_DS_CPL       = 1 << 4,
    ECX_VMX          = 1 << 5,
    ECX_SMX          = 1 << 6,
    ECX_EST          = 1 << 7,
    ECX_TM2          = 1 << 8,
    ECX_SSSE3        = 1 << 9,
    ECX_CID          = 1 << 10,
    ECX_SDBG         = 1 << 11,
    ECX_FMA          = 1 << 12,
    ECX_CX16         = 1 << 13,
    ECX_XTPR         = 1 << 14,
    ECX_PDCM         = 1 << 15,
    ECX_PCID         = 1 << 17,
    ECX_DCA          = 1 << 18,
    ECX_SSE4_1       = 1 << 19,
    ECX_SSE4_2       = 1 << 20,
    ECX_X2APIC       = 1 << 21,
    ECX_MOVBE        = 1 << 22,
    ECX_POPCNT       = 1 << 23,
    ECX_TSC          = 1 << 24,
    ECX_AES          = 1 << 25,
    ECX_XSAVE        = 1 << 26,
    ECX_OSXSAVE      = 1 << 27,
    ECX_AVX          = 1 << 28,
    ECX_F16C         = 1 << 29,
    ECX_RDRAND       = 1 << 30,
    ECX_HYPERVISOR   = 1 << 31,

    EDX_FPU          = 1 << 0,
    EDX_VME          = 1 << 1,
    EDX_DE           = 1 << 2,
    EDX_PSE          = 1 << 3,
    EDX_TSC          = 1 << 4,
    EDX_MSR          = 1 << 5,
    EDX_PAE          = 1 << 6,
    EDX_MCE          = 1 << 7,
    EDX_CX8          = 1 << 8,
    EDX_APIC         = 1 << 9,
    EDX_SEP          = 1 << 11,
    EDX_MTRR         = 1 << 12,
    EDX_PGE          = 1 << 13,
    EDX_MCA          = 1 << 14,
    EDX_CMOV         = 1 << 15,
    EDX_PAT          = 1 << 16,
    EDX_PSE36        = 1 << 17,
    EDX_PSN          = 1 << 18,
    EDX_CLFLUSH      = 1 << 19,
    EDX_DS           = 1 << 21,
    EDX_ACPI         = 1 << 22,
    EDX_MMX          = 1 << 23,
    EDX_FXSR         = 1 << 24,
    EDX_SSE          = 1 << 25,
    EDX_SSE2         = 1 << 26,
    EDX_SS           = 1 << 27,
    EDX_HTT          = 1 << 28,
    EDX_TM           = 1 << 29,
    EDX_IA64         = 1 << 30,
    EDX_PBE          = 1 << 31
};

/**
 * @brief Check if this processor supports model-specific registers.
 * 
 * @return true if MSR is supported, false otherwise.
 */
bool hasMSR();

/**
 * @brief Read from a model-specific register (MSR).
 *
 * @param msr The MSR address to read from.
 * @param low Pointer to store the low 32 bits of the MSR value.
 * @param high Pointer to store the high 32 bits of the MSR value.
 */
void readMSR(uint32_t msr, uint32_t* low, uint32_t* high);

/**
 * @brief Write to a model-specific register (MSR).
 *
 * @param msr The MSR address to write to.
 * @param low The low 32 bits of the value to write.
 * @param high The high 32 bits of the value to write.
 */
void writeMSR(uint32_t msr, uint32_t low, uint32_t high);

/**
 * @brief Check if this processor has a local APIC.
 * 
 * @return true if local APIC is supported, false otherwise.
 */
bool hasLocalAPIC();

/**
 * @brief Check if this processor has a page-attribute table.
 */
bool hasPAT();

/**
 * @brief Get the memory-mapped physical base address of the local APIC.
 * 
 * @return The physical base address of the local APIC.
 */
uintptr_t localAPICBaseAddress();

/**
 * @brief Enable the local APIC through a MSR and set the physical base address.
 * 
 * @param physicalBaseAddress The physical address to set for the local APIC.
 */
void hardwareEnableLocalAPICAndSetBaseAddress(uintptr_t physicalBaseAddress);

/**
 * @brief Send a byte value to the specified I/O port.
 *
 * @param port The I/O port number.
 * @param value The byte value to send.
 */
void outb(uint16_t port, uint8_t value);

/**
 * @brief Read a byte value from the specified I/O port.
 *
 * @param port The I/O port number.
 * @return The byte value read from the I/O port.
 */
uint8_t inb(uint16_t port);

/**
 * @brief Wait for an I/O operation to complete, usually about 1-3 microseconds.
 */
void ioWait();

/**
 * @brief Remap the PIC to not conflict with the local APIC.
 *
 * @param offset1 New offset for the master PIC.
 * @param offset2 New offset for the slave PIC.
 */
void remapPIC(int offset1, int offset2);

/**
 * @brief Disable the PIC by masking all interrupts.
 */
void disablePIC();

} // namespace Processor


