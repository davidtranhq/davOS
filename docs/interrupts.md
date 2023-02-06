# Setting up Interrupts

* byte representations of the IDT and the IDT descriptor are defined in `IDTStructure.h` and `TableDescriptor.h`
* IDT and IDT Descriptor are stored in the bss memory segment of the kernel (located somewhere above `0xffffffff80000000` in virtual memory)
* IMPORTANT: segment selector in the interrupt gate points to a readable code segment defined in the GDT (see the limine protocol for the GDT)

After a call to `LIDT addr`, where `addr` is the address of the IDT descriptor, the IDT register points to the IDT at `0xffff ffff 8000 2d60` whose size is `0xfff + 1 = 0x1000`:
```
qemu> info registers
...
IDT=     ffffffff80002d60 00000fff
...
```
The IDT contains 256 descriptors (the following shows the first entry in the IDT):
```
qemu> memsave 0xffffffff80002d60 0x1000 mem.dump

mem.dump:
            0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
8000 2d60: 00 00 28 00 00 8E 00 80 FF FF FF FF 00 00 00 00
...
```
The descriptor defines the divide-by-zero interrupt, whose ISR is located at `0xffff ffff 8000 0000` (bytes 0-1 and 7-12 in little endian of the IDT entry):
```c++
$ objdump davos.elf -S  | grep -A 10 isr_divide_by_zero

__attribute__((interrupt))
void isr_divide_by_zero(void *interrupt_frame)
{
ffffffff80000000:       55                      push   %rbp
ffffffff80000001:       48 89 e5                mov    %rsp,%rbp
ffffffff80000004:       57                      push   %rdi
ffffffff80000005:       50                      push   %rax
ffffffff80000006:       48 83 ec 10             sub    $0x10,%rsp
ffffffff8000000a:       48 8d 45 08             lea    0x8(%rbp),%rax
ffffffff8000000e:       48 89 45 e8             mov    %rax,-0x18(%rbp)
    kernel_panic("divide by zero\n");
ffffffff80000012:       48 c7 c7 80 1c 00 80    mov    $0xffffffff80001c80,%rdi
ffffffff80000019:       b8 00 00 00 00          mov    $0x0,%eax
ffffffff8000001e:       fc                      cld
ffffffff8000001f:       e8 de 05 00 00          call   ffffffff80000602 <kernel_panic>

ffffffff80000024 <_Z8idt_initv>:
}
```

So when a divide-by-zero occurs:
```c++
int test_interrupt_divide_by_zero()
{
    int x = 0;
    return 1 / x;
}
```
a kernel panic is invoked.
```
KERNEL PANIC: divide by zero
```
