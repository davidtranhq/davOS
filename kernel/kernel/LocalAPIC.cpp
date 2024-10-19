#include <cstdint>
#include <kernel/constants.h>
#include <kernel/frame_allocator.h>
#include <kernel/LocalAPIC.hpp>
#include <kernel/paging.h>
#include <kernel/processor.hpp>

struct LocalAPIC::Registers {
    uint32_t reserved0[2];
    uint32_t id;
    uint32_t version;
    uint32_t reserved1[3];
    uint32_t taskPriority;
    uint32_t arbitrationPriority;
    uint32_t processorPriority;
    uint32_t eoi;
    uint32_t remoteRead;
    uint32_t logicalDestination;
    uint32_t destinationFormat;
    uint32_t spuriousInterruptVector;
    uint32_t isr[8];
    uint32_t tmr[8];
    uint32_t irr[8];
    uint32_t errorStatus;
    uint32_t reserved2[7];
    uint32_t lvtCMCI;
    uint32_t interruptCommand0;
    uint32_t interruptCommand1;
    uint32_t lvtTimer;
    uint32_t lvtThermalSensor;
    uint32_t lvtPerformanceMonitor;
    uint32_t lvtLINT0;
    uint32_t lvtLINT1;
    uint32_t lvtError;
    uint32_t initialCount;
    uint32_t currentCount;
    uint32_t reserved3[4];
    uint32_t divideConfiguration;
    uint32_t reserved4[3];
    uint32_t timer1InitialCount;
    uint32_t timer1CurrentCount;
    uint32_t reserved5[2];
    uint32_t timer2InitialCount;
    uint32_t timer2CurrentCount;
    uint32_t reserved6[2];
    uint32_t lvtPerformanceCounter;
    uint32_t reserved7[2];
    uint32_t lvtLINT0InitialCount;
    uint32_t lvtLINT1InitialCount;
    uint32_t lvtErrorInitialCount;
    uint32_t reserved8[4];
    uint32_t timer1DivideConfiguration;
    uint32_t timer2DivideConfiguration;
    uint32_t reserved9[2];
    uint32_t lvtPerformanceCounterDivideConfiguration;
};

void LocalAPIC::enableAPIC()
{
    const auto physicalAPICAddress = processor::localAPICBaseAddress();
    const auto virtualAPICAddress = kernel_physical_to_virtual(physicalAPICAddress);
    paging_add_mapping(virtualAPICAddress, physicalAPICAddress, kernelConstants::pageSize, PageFlags::Write | PageFlags::CacheDisable);
    m_baseAddress = reinterpret_cast<Registers*>(physicalAPICAddress);
    processor::hardwareEnableLocalAPICAndSetBaseAddress(physicalAPICAddress);

    // In addition to disabling the PIC, we also remap the PIC1 and PIC2 interrupt requests 
    // to start at 0x20 and 0x28 respectively instead of 0x8 and 0x70. This is done to avoid
    // avoid conflicts with the local APIC. Even though the PIC is disabled, it
    // could still give out spurious interrupts, which would conflict with system exceptions.
    processor::disablePIC();
    processor::remapPIC(0x20, 0x28);

    
    // The spurious interrupt vector is the vector that the local APIC will use to signal interrupts
    // that are not associated with a specific interrupt vector. This is typically set to 0xFF.
    // The 16th bit of the spurious interrupt vector is the APIC software enable bit.
    // This bit must be set to 1 to enable the local APIC.
    m_baseAddress->spuriousInterruptVector = 0x1FF;
}
