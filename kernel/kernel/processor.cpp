#include <cpuid.h>
#include <cstdint>
#include <kernel/kernel.h>
#include <kernel/processor.hpp>


/**
 * @brief Check if this processor supports model-specific registers.
 */
bool processor::hasMSR()
{
    uint32_t eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & static_cast<uint32_t>(CpuIdFeature::EDX_MSR);
}

/**
 * @brief Read from a model-specific register.
 */
void processor::readMSR(uint32_t msr, uint32_t* low, uint32_t* high)
{
    if (!hasMSR())
        kernel_panic("processor does not support MSR");
    asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(msr));
}

/**
 * @brief Write to a model-specific register.
 */
void processor::writeMSR(uint32_t msr, uint32_t low, uint32_t high)
{
    if (!hasMSR())
        kernel_panic("processor does not support MSR");
    asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}

/**
 * @brief Check if this processor has a local APIC.
 */
bool processor::hasLocalAPIC()
{
    uint32_t eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & static_cast<uint32_t>(CpuIdFeature::EDX_APIC);
}

bool processor::hasPAT()
{
    uint32_t eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & static_cast<uint32_t>(CpuIdFeature::EDX_PAT);
}

/**
 * @brief Get the memory-mapped physical base address of the local APIC.
 * The address is aligned to a 4 KiB boundary.
 */
uintptr_t processor::localAPICBaseAddress()
{
    if (!hasLocalAPIC())
        kernel_panic("processor does not support local APIC");
    uint32_t low, high;
    readMSR(0x1B, &low, &high);
#ifdef __PHYSICAL_MEMORY_EXTENSION__
    return static_cast<uint64_t>(low & 0xfffff000) | (static_cast<uint64_t>((high & 0x0f)) << 32);
#else
    return low & 0xfffff000;
#endif
}

/**
 * @brief Enable the local APIC through a MSR and set the physical base address.
 *
 * If this is not enabled (i.e. by setting the enable bit in the IA32_APIC_BASE MSR,
 * the processor is functionally equivalent to a processor without an on-chip APIC.
 *
 * Note that after calling this function the local APIC is still not ready to receive
 * interrupts. The software must also set the spurious interrupt vector.
 */
void processor::hardwareEnableLocalAPICAndSetBaseAddress(uintptr_t physicalBaseAddress)
{
    if (!hasLocalAPIC())
        kernel_panic("processor does not support local APIC");
    uint32_t high = 0;
    constexpr uint32_t ia32ApicBaseEnable = 1 << 11;
    uint32_t low = (physicalBaseAddress & 0xfffff000) | ia32ApicBaseEnable;

#ifdef __PHYSICAL_MEMORY_EXTENSION__
    high = (physicalBaseAddress >> 32) & 0x0f;
#endif

    writeMSR(0x1B, low, high);
}

/**
 * @brief Send a byte value to the specified I/O port.
 */
void processor::outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

/**
 * @brief Read a byte value from the specified I/O port.
 */
uint8_t processor::inb(uint16_t port)
{
    uint8_t value;
    asm volatile ("inb %w1, %b0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

/**
 * @brief Wait for an I/O operation to complete, usually about 1-3 microseconds.
 */
void processor::ioWait()
{
    outb(0x80, 0);
}

/**
 * @brief Remap the PIC to not conflict with the local APIC.
 *
 * The PIC uses IRQs 0-7 and IRQs 8-15. The local APIC uses IRQs 16-23.
 * This function remaps the PIC to use IRQs 32-39 and IRQs 40-47.
 */
void processor::remapPIC(int offset1, int offset2) {
    // Initialization Control Word (ICW) constants
    constexpr uint8_t ICW1_ICW4 = 0x01;         // Indicates that ICW4 will be present
    constexpr uint8_t ICW1_Init = 0x10;           // Initialization - required!
    constexpr uint8_t ICW4_8086 = 0x01;           // 8086/88 (MCS-80/85) mode

    // I/O port addresses for PIC
    constexpr uint16_t PIC1Command = 0x20; // Command port for PIC1
    constexpr uint16_t PIC1Data = 0x21;    // Data port for PIC1
    constexpr uint16_t PIC2Command = 0xA0; // Command port for PIC2
    constexpr uint16_t PIC2Data = 0xA1;    // Data port for PIC2

    uint8_t a1 = inb(PIC1Data);  // Save masks for PIC1
    uint8_t a2 = inb(PIC2Data);  // Save masks for PIC2

    outb(PIC1Command, ICW1_Init | ICW1_ICW4); // Start the initialization sequence (in cascade mode)
    ioWait();

    outb(PIC2Command, ICW1_Init | ICW1_ICW4);
    ioWait();

    outb(PIC1Data, offset1);     // ICW2: Master PIC vector offset
    ioWait();

    outb(PIC2Data, offset2);     // ICW2: Slave PIC vector offset
    ioWait();

    outb(PIC1Data, 4);           // ICW3: Tell Master PIC that there is a Slave PIC at IRQ2
    ioWait();

    outb(PIC2Data, 2);           // ICW3: Tell Slave PIC its cascade identity
    ioWait();

    outb(PIC1Data, ICW4_8086);   // ICW4: Set the PICs to use 8086 mode
    ioWait();

    outb(PIC2Data, ICW4_8086);
    ioWait();

    outb(PIC1Data, a1);          // Restore saved masks for PIC1
    outb(PIC2Data, a2);          // Restore saved masks for PIC2
}

/**
 * @brief Disable the PIC by masking all interrupts.
 */
void processor::disablePIC()
{
    constexpr uint16_t pic1DataPort = 0x21;
    constexpr uint16_t pic2DataPort = 0xA1;
    outb(pic1DataPort, 0xff);
    outb(pic2DataPort, 0xff);
}

/**
 * @brief Initialize the PS/2 keyboard controller to enable IRQ1 generation.
 * QEMU should do this automatically, but in some stricter QEMU environments
 * or on actual hardware, this manual configuration might be necessary.
 */
void processor::initKeyboardController()
{
    // Wait until the input buffer is clear
    while (inb(0x64) & 0x02);

    // Enable keyboard device (send command 0xAE to controller)
    outb(0x64, 0xAE);
    ioWait();

    // Request current command byte
    while (inb(0x64) & 0x02);  // Wait for input buffer to clear
    outb(0x64, 0x20);          // Read command byte
    ioWait();

    // Wait for output buffer to be ready
    while (!(inb(0x64) & 0x01));
    uint8_t commandByte = inb(0x60);

    // Enable IRQ1 (bit 0), and keyboard clock (clear bit 4)
    commandByte |= 0x01;   // Enable IRQ1
    commandByte &= ~0x10;  // Enable keyboard clock

    // Write command byte back
    while (inb(0x64) & 0x02);
    outb(0x64, 0x60);  // Write command byte command
    ioWait();

    while (inb(0x64) & 0x02);
    outb(0x60, commandByte);
    ioWait();
}

