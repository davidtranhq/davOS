# A Tour of Operating Systems

## Booting
1. The computer is turned on.
1. The computer runs through hardware diagnostics (Power-On Self Test, or POST).
1. The computer locates a bootable device (e.g. a hard drive, CD-ROM, USB flash drive, etc.).
1. The computer runs the bootloader (executable code stored on the bootable device), which loads the kernel into memory, configures an environnment for the kernel, and finally passes control to the kernel.

### x86
* the bootloader may also initialize the Global Descriptor Table (GDT), a table of entries where each entry contains information for a memory segment
    * if the bootloader does not do this, it must be done by the kernel
    * GDT is required to run in Long or Protected Mode (the main operating mode of x86 processors)

## Interrupts (x86)
* **Interrupt Descriptor Table (IDT)**: table stored in memory containing the memory addresses of interrupt service routines (ISRs)
* **IDT Descriptor**: data structure stored in memory containing the size and address of the IDT
* IDT location is stored in the `IDTR` register, loaded by the `LIDT <address_to_IDT_descriptor>` assembly instruction
* **vectored events**: interrupts and exceptions; events that cause the CPU to jump to an ISR located in the IDT
    * interrupts and exceptions have an associated ID (vector) that tells the offset of the ISR in the IDT
    * **interrupt**: occurs due to a signal from hardware
    * **exception**: occurs due to a CPU runtime error (e.g. divide by zero)
        * **trap**: exception that skips the offending instruction after returning from the ISR
        * **fault**: exception that retries the offending instruction after returning
        * **abort**: severe exception, does not return
