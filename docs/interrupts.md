# Setting up Interrupts

* byte representations of the IDT and the IDT descriptor are defined in `IDT.h` and `IDTDescriptor.h`
* IDT and IDTDescriptor are stored in the bss memory segment of the kernel (located somewhere above `0xffffffff80000000` in virtual memory)
* IMPORTANT: segment selector in the interrupt gate points to a readable code segment defined in the GDT (see the limine protocol for the GDT)
