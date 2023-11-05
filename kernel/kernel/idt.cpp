#include <string.h>

#include <kernel/idt.h>
#include <kernel/IDTStructure.h>
#include <kernel/TableDescriptor.h>
#include <kernel/kernel.h>
#include <kernel/SegmentSelector.h>

#include <frg/array.hpp> // std::size

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
}

__attribute__((interrupt))
void isr_debug_exception(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("debug exception\n");
}

__attribute__((interrupt))
void isr_nmi_interrupt(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("nonmaskable external interrupt\n");
}

#ifdef INTERRUPT_TEST
/**
 * @brief The breakpoint interrupt handler is used for testing interrupt handling
 * in the test build.
 */
__attribute__((interrupt))
void isr_breakpoint(IDTStructure::InterruptFrame *frame_arg)
{
    printf("interrupt handling test: PASSED\n");
}
#else
__attribute__((interrupt))
void isr_breakpoint(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("breakpoint\n");
}
#endif

__attribute__((interrupt))
void isr_overflow(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("overflow\n");
}

__attribute__((interrupt))
void isr_bound_range_exceeded(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("bound range exceeded\n");
}

__attribute__((interrupt))
void isr_invalid_opcode(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("invalid opcode\n");
}

__attribute__((interrupt))
void isr_device_not_available(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("device not available\n");
}

__attribute__((interrupt))
void isr_double_fault(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("double fault\n");
}

__attribute__((interrupt))
void isr_coprocessor_segment_overrun(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("coprocessor segment overrun\n");
}

__attribute__((interrupt))
void isr_invalid_tss(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("invalid tss\n");
}

__attribute__((interrupt))
void isr_segment_not_present(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("segment not present\n");
}

__attribute__((interrupt))
void isr_stack_segment_fault(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("stack segment fault\n");
}

__attribute__((interrupt))
void isr_general_protection(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("general protection\nerror_code: %x", error_code);
}

__attribute__((interrupt))
void isr_page_fault(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    const char *p_flag_msg = error_code & 1
        ? "The fault was caused by a page-level protection violation"
        : "The fault was caused by a non-present page";
    
    const char *wr_flag_msg = error_code & 2
        ? "The access causing the fault was a write"
        : "The access causing the fault was a read";

    const char *us_flag_msg = error_code & 4
        ? "A user-mode access caused the fault"
        : "A supervisor-mode access caused the fault";

    const char *rsvd_flag_msg = error_code & 8
        ? "The fault was not caused by a reserved bit violation"
        : "The fault was caused by a reserved bit set to 1 in some paging-structure entry";

    const char *id_flag_msg = error_code & 16
        ? "The fault was caused by an instruction fetch"
        : "The fault was not caused by an instruction fetch";

    const char *pk_flag_msg = error_code & 32
        ? "There was a protection-key violation"
        : "The fault was not caused by protection keys";

    const char *sgx_flag_msg = error_code & (1ULL << 15)
        ? "The fault resulted from violation of SGX-specific access-control requirements"
        : "The fault is not related to SGX";

    kernel_panic(
        "page fault\n\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
        p_flag_msg, wr_flag_msg, us_flag_msg, rsvd_flag_msg, id_flag_msg, pk_flag_msg, sgx_flag_msg
    );

}

__attribute__((interrupt))
void isr_math_fault(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("math fault\n");
}

__attribute__((interrupt))
void isr_alignment_check(IDTStructure::InterruptFrame *frame, uint64_t error_code)
{
    kernel_panic("alignment check\n");
}

__attribute__((interrupt))
void isr_machine_check(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("machine check\n");
}

__attribute__((interrupt))
void isr_simd_floating_point_exception(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("SIMD floating-point exception\n");
}

__attribute__((interrupt))
void isr_virtualization_exception(IDTStructure::InterruptFrame *frame)
{
    kernel_panic("virtualization exception\n");
}

IDTStructure idt;
TableDescriptor idt_descriptor(IDTStructure::size, idt.address());

} // anonymous namespace

void idt_init()
{
    IDTStructure::GateDescriptor idt_descriptors[] = {
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
    };
    
    for (size_t i = 0; i < std::size(idt_descriptors); ++i)
    {
        idt.load_gate_descriptor(i, idt_descriptors[i]);
    }

    // load the address of the IDTStructure Descriptor into the IDTR (IDT register)
    __asm__("lidt %0" :: "m"(*idt_descriptor.address()));

}