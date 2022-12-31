# Memory


(The following uses the project as-of commit [0f06c82](https://github.com/davidtranhq/davOS/tree/0f06c82e6cd5e7df535993fc7e3ba374ed64d1ed)).

Hex addresses are implicitly virtual unless suffixed by `p` (e.g. `0x1000` is virtual, `0x1000p` is physical).

The Limine boot protocol sets up paging and initializes page tables that are meant to be overridden by our own page tables. I describe in more detail why and how Limine memory-mapping works, since the [Limine protocol](https://github.com/limine-bootloader/limine/blob/trunk/PROTOCOL.md) assumes a good amount of prior knowledge about bootloaders.

## Kernel Layout

A summary of the headers in the `davos.elf` executable:

```console
$ objdump davos.elf --headers

davos.elf:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000e21  ffffffff80000000  ffffffff80000000  00001000  2**1
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .rodata       000001c8  ffffffff80001e28  ffffffff80001e28  00001e28  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  2 .init_array   00000008  ffffffff80002ff0  ffffffff80002ff0  00002ff0  2**3
                  CONTENTS, ALLOC, LOAD, DATA
  3 .data         000002f8  ffffffff80003000  ffffffff80003000  00003000  2**5
                  CONTENTS, ALLOC, LOAD, DATA
  4 .bss          00001018  ffffffff80003300  ffffffff80003300  000032f8  2**5
                  ALLOC
```
* note that the kernel is loaded starting at `0xffffffff800000000` (a higher-half kernel) and ranges from `0x...0000` to `0x...4318` (`0x...4318` is obtained by adding the size of the  `.bss` section, `0x1018`, to the start of this section, `0x...3300`)
    * as the size of the kernel is `0x4318`, it spans 5 `0x1000` sized pages

## Limine Memory Mapping

The Limine boot protocol initializes the `cr0`, `cr4`, and `EFER` registers to values that enable IA-32e paging (Intel Long Mode paging). Paging (or any sort of memory virtualization) makes it possible to load a higher-half kernel: without it, the kernel address of `0xffffffff80000000` would have to be physical, which would require a system with over 16 PiB of memory.

### Limine Paging

Limine implements virtualiztion using paging. The bootloader loads the base address of the initial page directory into the `cr3` register and initializes the page directory and the corresponding page tables to achieve the following virtual-to-physical mapping:

```python
def virtual_to_physical(addr):
    # the kernel base address is mandated by the protocol to be at least 0xfffffff80000000
    kernel_base_address = 0xffffffff80000000
    if addr in range(kernel_base_address, kernel_base_address + kernel_limit):
        return virtual_to_physical_kernel_mapping(addr)

    four_gibibytes = 0x100000000
    if addr in range(0x1000, 0x1000 + four_gibibytes):
        return addr
    # HHDM start is an address indicating the start of the higher-half memory region
    if addr in range(hhdm_start, hhdm_start + four_gibibytes):
        return addr - hhdm_start
    # the memory map contains other memory sections used/initialized by the bootloader that aren't the kernel (i.e. includes the stack)
    for entry in memory_map:
        if addr in range(entry.base, entry.base + entry.length):
            return addr
        if addr in range(hhdm_start + entry.base, hhdm_start + entry.base + entry.length):
            return addr - hhdm_start
    
    raise PageFaultException
```
That is:
* virtual addresses that are within the virtual address space of the kernel are mapped to a location in physical memory not specified by the protocol
    * the physical location of the kernel can be obtained at runtime with the Limine Kernel Address Feature
* virtual addresses that are within 4 GiB of `0x1000` are identity mapped to the same physical address
* virtual addresses that are above the HHDM base are translated to (virtual - HHDM base)
    * e.g. if `hhdm_base = 0xffff800000000000`, then `0xffff800000001000` maps to `0x1000p`
    * the HHDM base address is also unspecified by the protocol but can be obtained at runtime with the Limine Higher Half Direct Map Feature
    * Note that the HHDM base address is less than and NOT equal to the kernel base address of `0xffffffff80000000`. The region of memory between the HHDM base address and the kernel base address is unspecified by the protocol.
* virtual addresses that are located within a memory-mapped region BELOW the HHDM are identity mapped to the same physical address
* virtual addresses that are located within a memory-mapped region ABOVE the HHDM are translated to (virtual - HHDM base)

#### Page Directory In Memory

The `info mem` command in the QEMU monitor shows the virtual addresses of the mapped regions in the page directory pointed to by the `cr3` register:

```console
qemu> info mem
0000000000001000-0000000100000000 00000000fffff000 -rw
ffff800000000000-ffff800100000000 0000000100000000 -rw
ffffffff80000000-ffffffff80003000 0000000000003000 -r-
ffffffff80003000-ffffffff80005000 0000000000002000 -rw
```

The format is `<base>-<limit> <size> <permissions>` where the permissions are `[urw]` and `u = user mode accessible`, `r = readable`, `w = writable`.

The 1st entry is the identity mapping. The 2nd is the HHDM mapping. The 3rd is the `.text`, `.rodata`, and `.init_array` sections of the kernel (note that the permissions indicate the page is read-only). The 4th is the `.data` and `.bss` sections of the kernel (read-write).

The `info tlb` command shows the actual virtual-to-physical translations cached in the TLB.

```console
qemu> info tlb
0000000000001000: 0000000000001000 --------W //
...                                          // identity mapping
00000000ffe00000: 00000000ffe00000 --P-----W //

ffff800000000000: 0000000000000000 --------W //
...                                          // HHDM mapping
ffff8000ffe00000: 00000000ffe00000 --P-----W //

ffffffff80000000: 0000000007c2a000 --------- //
ffffffff80001000: 0000000007c2b000 X-------- //
ffffffff80002000: 0000000007c2c000 X-------- // kernel mapping
ffffffff80003000: 0000000007c2d000 X-------W //
ffffffff80004000: 0000000007c2e000 X-------W //
```
* in this case, the kernel is mapped to `0x7c2a000p` in physical memory

#### Why Physical Addresses are Mapped Twice
Some physical addresses are mapped to twice: once through the identity mapping and once through the HHDM mapping. For example, both `0xffff800000001000` and `0x1000` map to `0x1000p`. This is used by the bootloader to solve the issue that arises when enabling paging. Consider the following routine that enables paging:
```arm
enable_paging:
0x1000p ENABLE_PAGING ; <-- paging is off here
0x1001p JUMP_TO_HIGHER_HALF ; <-- paging is on now!
```
When the instruction pointer points to `0x1000p`, paging is disabled, so the value in the IP is a physical address. But, once paging is enabled, the address pointed to by the IP becomes a virtual address. If the identity mapping were not loaded into the page table, an attempt by the CPU to read from `0x1001` (after incrementing the IP from `0x1000`) would result in a page fault, since `0x1001` is not mapped to a physical address. With the identity mapping, `0x1001` correctly maps to `0x1001p`, and the CPU can safely read subsequent instructions (later jumping to the kernel located in the higher half of virtual memory).

## Replacing Limine's Page Tables

The Limine bootloader page tables serve the purpose of entering Protected Mode and intiailizing the higher-half kernel. We replace them with our own to:

1. Reclaim memory used by the bootloader that is no longer needed
2. Make it easier to manage the page table ourselves when we want to add new entries to it

Replacing the page tables with our own involves:
1. Initializing the page directory and page tables in memory that map:
    * all kernel frames
    * frame(s) containing the stack and other frames containing data needed by the kernel
    * any other memory-mapped pages initialized in Limine's page tables that we might want to keep (e.g. the page mapping the Limine terminal to the physical framebuffer)
2. Load the location of the new page directory into the `cr3` register

