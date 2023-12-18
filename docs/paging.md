# Paging

When the kernel is loaded, Limine enables IA-32e paging with write-protection (pages with read-only access rights are protected from supervisor-mode writes). Also, IA32_EFER.NXE is enabled: this enables page access restrictions by preventing instruction fetches from pages with the XD bit 63 set.

Every paging structure is 4096 bytes in size comprised of 512 entries (64 bits each).

## IA-32e Paging

* initial paging structure located using the contents of cr3
* translates 48-bit linear addresses to 52-bit physical addreses

CRL3 -> PML4E -> Page-Directory-Pointer Table (PDPT) -> Page-Directory -> Page Table 

Initially, we disable process-context identifiers (PCIDs).

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

Writing to the `cr3` register will automatically flush the TLB. Note that each page table/directory entry needs to be 12-bit aligned, since the entry contains the address of its child in bits 0-12, but the bottom 8 bits are used for flags.