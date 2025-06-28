#include <kpp/cstring.hpp>
#include <cstddef>

#include <kernel/constants.h>
#include <kernel/frame_allocator.h>
#include <kernel/idt.h>
#include <kernel/IDTStructure.h>
#include <kernel/kernel.h>
#include <kernel/KeyboardBuffer.hpp>
#include <kernel/paging.h>
#include <kernel/processor.hpp>
#include <kernel/SegmentSelector.h>
#include <kernel/TableDescriptor.h>
#include <kernel/Terminal.hpp>
#include <kpp/Array.hpp>

#include <kpp/cstdio.hpp>

#define INTERPROCESSOR_INTERRUPT_TEST
#ifdef TEST_BUILD
#include <stdio.h>
#endif

namespace
{

/**
 * See Chapter 6.3.1 of the Intel 64 IA-32 architecture manual for more information
 * of the following interrupt service routines.
 */

__attribute__((interrupt))
void isr_divide_error(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("divide error\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_debug_exception(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("debug exception\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_nmi_interrupt(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("nonmaskable external interrupt\n");
    processor::localAPIC.sendEndOfInterrupt();
}

#ifdef INTERRUPT_TEST
/**
 * @brief The breakpoint interrupt handler is used for testing interrupt handling
 * in the test build.
 */
__attribute__((interrupt))
void isr_breakpoint(IDTStructure::InterruptFrame *frame_arg)
{
    kpp::printf("interrupt handling test: PASSED\n");
    processor::localAPIC.sendEndOfInterrupt();
}
#else
__attribute__((interrupt))
void isr_breakpoint(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("breakpoint\n");
    processor::localAPIC.sendEndOfInterrupt();
}
#endif

__attribute__((interrupt))
void isr_overflow(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("overflow\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_bound_range_exceeded(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("bound range exceeded\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_invalid_opcode(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("invalid opcode\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_device_not_available(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("device not available\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_double_fault(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("double fault\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_coprocessor_segment_overrun(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("coprocessor segment overrun\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_invalid_tss(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("invalid tss\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_segment_not_present(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("segment not present\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_stack_segment_fault(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("stack segment fault\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_general_protection(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("general protection\nerror_code: %x", error_code);
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_page_fault(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
     /*const char *p_flag_msg = error_code & 1*/
     /*    ? "The fault was caused by a page-level protection violation"*/
     /*    : "The fault was caused by a non-present page";*/
     /**/
     /*const char *wr_flag_msg = error_code & 2*/
     /*    ? "The access causing the fault was a write"*/
     /*    : "The access causing the fault was a read";*/
     /**/
     /*const char *us_flag_msg = error_code & 4*/
     /*    ? "A user-mode access caused the fault"*/
     /*    : "A supervisor-mode access caused the fault";*/
     /**/
     /*const char *rsvd_flag_msg = error_code & 8*/
     /*    ? "The fault was not caused by a reserved bit violation"*/
     /*    : "The fault was caused by a reserved bit set to 1 in some paging-structure entry";*/
     /**/
     /*const char *id_flag_msg = error_code & 16*/
     /*    ? "The fault was caused by an instruction fetch"*/
     /*    : "The fault was not caused by an instruction fetch";*/
     /**/
     /*const char *pk_flag_msg = error_code & 32*/
     /*    ? "There was a protection-key violation"*/
     /*    : "The fault was not caused by protection keys";*/
     /**/
     /*const char *sgx_flag_msg = error_code & (1ULL << 15)*/
     /*    ? "The fault resulted from violation of SGX-specific access-control requirements"*/
     /*    : "The fault is not related to SGX";*/
     /**/
     /*kernel_panic(*/
     /*    "page fault\n\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",*/
     /*    p_flag_msg, wr_flag_msg, us_flag_msg, rsvd_flag_msg, id_flag_msg, pk_flag_msg, sgx_flag_msg*/
     /*);*/

    auto faulting_address = uintptr_t {};
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));
    paging_allocate_and_map(faulting_address, kernelConstants::pageSize, PageFlags::Write);
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_math_fault(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("math fault\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_alignment_check(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("alignment check\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_machine_check(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("machine check\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_simd_floating_point_exception(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("SIMD floating-point exception\n");
    processor::localAPIC.sendEndOfInterrupt();
}

__attribute__((interrupt))
void isr_virtualization_exception(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("virtualization exception\n");
    processor::localAPIC.sendEndOfInterrupt();
}

#ifdef INTERPROCESSOR_INTERRUPT_TEST
__attribute__((interrupt))
void isr_interprocessor_interrupt(IDTStructure::InterruptFrame *frame)
{
    kpp::printf("interprocessor interrupt test: PASSED\n");
    processor::localAPIC.sendEndOfInterrupt();
}
#endif

__attribute__((interrupt))
void isr_keyboard(IDTStructure::InterruptFrame *frame)
{
    auto scan_code = processor::inb(0x60);

    using Key = KeyboardBuffer::Key;
    auto scancode_to_key = [](uint8_t scancode) -> Key {
        // Handle extended key prefix
        static bool extended = false;
        if (extended) {
            extended = false;
            switch (scancode) {
                case 0x48: return Key::UpArrow;
                case 0x50: return Key::DownArrow;
                case 0x4B: return Key::LeftArrow;
                case 0x4D: return Key::RightArrow;
                default:   return Key::None;
            }
        }

        if (scancode == 0xE0) {
            extended = true;
            return Key::None;
        }

        if (scancode & 0x80) {
            // Ignore key release
            return Key::None;
        }

        // Use a simple scancode-to-ASCII table
        static const char scancode_to_ascii[128] = {
            0, 27, '1', '2', '3', '4', '5', '6',  // 0x00 - 0x07
            '7', '8', '9', '0', '-', '=', '\b', '\t',
            'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
            'o', 'p', '[', ']', '\n', 0,  'a', 's',
            'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
            '\'', '`', 0,  '\\', 'z', 'x', 'c', 'v',
            'b', 'n', 'm', ',', '.', '/', 0,   '*',
            0,  ' ', 0, 0, 0, 0, 0, 0,
            // (Fill in more if needed)
        };

        if (scancode < 128) {
            char ascii = scancode_to_ascii[scancode];
            if (ascii != 0) {
                return static_cast<Key>(ascii); // <-- The magic line
            }
        }

        return Key::None;
    };
    kernel::keyboardBuffer.put(scancode_to_key(scan_code));

    processor::localAPIC.sendEndOfInterrupt();
}

IDTStructure idt;
TableDescriptor idt_descriptor(IDTStructure::size, idt.address());

} // anonymous namespace

void idt_init()
{
    constexpr auto system_idt_descriptors = kpp::Array<IDTStructure::GateDescriptor, 20> {{
        {
            isr_divide_error, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            isr_debug_exception, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            isr_nmi_interrupt,
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            isr_breakpoint,
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::trap,
            PrivilegeLevel::kernel
        },
        {
            isr_overflow, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            isr_bound_range_exceeded,
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            isr_invalid_opcode, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            isr_device_not_available, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            {.with_error_code = isr_double_fault},
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            {.with_error_code = isr_coprocessor_segment_overrun}, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            {.with_error_code = isr_invalid_tss}, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            {.with_error_code = isr_segment_not_present},
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            {.with_error_code = isr_stack_segment_fault}, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            {.with_error_code = isr_general_protection}, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            {.with_error_code = isr_page_fault}, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            isr_math_fault, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            {.with_error_code = isr_alignment_check}, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            isr_machine_check, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            isr_simd_floating_point_exception, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        },
        {
            isr_virtualization_exception, 
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        }
    }};

    // User-defined interrupt vectors ("User" in this sense doesn't refer to privilege level, but rather "non-builtin")
    constexpr auto user_idt_descriptors = kpp::Array<IDTStructure::GateDescriptor, 1> {{
        {
            isr_keyboard,
            SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
            0,
            IDTStructure::GateType::interrupt,
            PrivilegeLevel::kernel
        }
    }};

    
    for (size_t i = 0; i < system_idt_descriptors.size(); ++i)
        idt.load_gate_descriptor(i, system_idt_descriptors[i]);

    // Note: Even though the user-defined interrupt vectors start at 0x20, the vectors
    // 0x20-0x2f are used as "sinks" for the spurious interrupts that might be generated
    // by the PIC (even if the PIC is disabled!). So we start at 0x30.
    
    for (size_t i = 0; i < user_idt_descriptors.size(); ++i)
        idt.load_gate_descriptor(i + 0x30, user_idt_descriptors[i]);

#ifdef INTERPROCESSOR_INTERRUPT_TEST
    idt.load_gate_descriptor(0xff, {
        isr_interprocessor_interrupt,
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::kernel_code),
        0,
        IDTStructure::GateType::interrupt,
        PrivilegeLevel::kernel
    });
#endif

    // load the address of the IDTStructure Descriptor into the IDTR (IDT register)
    __asm__("lidt %0" :: "m"(*idt_descriptor.address()));

}
