#include <string.h>

#include <kernel/idt.h>
#include <kernel/IDT.h>
#include <kernel/IDTDescriptor.h>
#include <kernel/kernel.h>
#include <kernel/SegmentSelector.h>

#include <frg/array.hpp> // std::size

namespace
{

/**
 * See Chapter 6.3.1 of the Intel 64 IA-32 architecture manual for more information
 * of the following interrupt service routines.
 */

__attribute__((interrupt))
void isr_divide_error(void *interrupt_frame)
{
    kernel_panic("divide error\n");
}

__attribute__((interrupt))
void isr_debug_exception(void *interrupt_frame)
{
    kernel_panic("debug exception\n");
}

__attribute__((interrupt))
void isr_nmi_interrupt(void *interrupt_frame)
{
    kernel_panic("nonmaskable external interrupt\n");
}

__attribute__((interrupt))
void isr_breakpoint(void *interrupt_frame)
{
    kernel_panic("breakpoint\n");
}

__attribute__((interrupt))
void isr_overflow(void *interrupt_frame)
{
    kernel_panic("overflow\n");
}

__attribute__((interrupt))
void isr_bound_range_exceeded(void *interrupt_frame)
{
    kernel_panic("bound range exceeded\n");
}

__attribute__((interrupt))
void isr_invalid_opcode(void *interrupt_frame)
{
    kernel_panic("invalid opcode\n");
}

__attribute__((interrupt))
void isr_device_not_available(void *interrupt_frame)
{
    kernel_panic("device not available\n");
}

__attribute__((interrupt))
void isr_double_fault(void *interrupt_frame)
{
    kernel_panic("double fault\n");
}

__attribute__((interrupt))
void isr_coprocessor_segment_overrun(void *interrupt_frame)
{
    kernel_panic("coprocessor segment overrun\n");
}

__attribute__((interrupt))
void isr_invalid_tss(void *interrupt_frame)
{
    kernel_panic("invalid tss\n");
}

__attribute__((interrupt))
void isr_segment_not_present(void *interrupt_frame)
{
    kernel_panic("segment not present\n");
}

__attribute__((interrupt))
void isr_stack_segment_fault(void *interrupt_frame)
{
    kernel_panic("stack segment fault\n");
}

__attribute__((interrupt))
void isr_general_protection(void *interrupt_frame)
{
    kernel_panic("general protection\n");
}

__attribute__((interrupt))
void isr_page_fault(void *interrupt_frame)
{
    kernel_panic("page fault\n");
}

__attribute__((interrupt))
void isr_math_fault(void *interrupt_frame)
{
    kernel_panic("math fault\n");
}

__attribute__((interrupt))
void isr_alignment_check(void *interrupt_frame)
{
    kernel_panic("alignment check\n");
}

__attribute__((interrupt))
void isr_machine_check(void *interrupt_frame)
{
    kernel_panic("machine check\n");
}

__attribute__((interrupt))
void isr_simd_floating_point_exception(void *interrupt_frame)
{
    kernel_panic("SIMD floating-point exception\n");
}

__attribute__((interrupt))
void isr_virtualization_exception(void *interrupt_frame)
{
    kernel_panic("virtualization exception\n");
}


IDT idt;
IDTDescriptor idt_descriptor(IDT::size, idt.address());

IDT::GateDescriptor idt_descriptors[] = {
    {
        isr_divide_error, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_debug_exception, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_nmi_interrupt,
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_breakpoint,
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_overflow, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_bound_range_exceeded,
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_invalid_opcode, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_device_not_available, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_double_fault,
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_coprocessor_segment_overrun, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_invalid_tss, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_segment_not_present, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_stack_segment_fault, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_general_protection, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_page_fault, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_math_fault, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_alignment_check, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_machine_check, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_simd_floating_point_exception, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    },
    {
        isr_virtualization_exception, 
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, GDTSegment::code64),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    }
};

} // anonymous namespace

void idt_init()
{
    for (size_t i = 0; i < std::size(idt_descriptors); ++i)
    {
        idt.load_gate_descriptor(i, idt_descriptors[i]);
    }

    // load the address of the IDT Descriptor into the IDTR (IDT register)
    __asm__("lidt %0" :: "m"(*idt_descriptor.address()));

}