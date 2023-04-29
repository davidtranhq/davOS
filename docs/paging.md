# Paging

When the kernel is loaded, Limine enables IA-32e paging with write-protection (pages with read-only access rights are protected from supervisor-mode writes). Also, IA32_EFER.NXE is enabled: this enables page access restrictions by preventing instruction fetches from pages with the XD bit 63 set.

Every paging structure is 4096 bytes in size comprised of 512 entries (64 bits each).

## IA-32e Paging

* initial paging structure located using the contents of cr3
* translates 48-bit linear addresses to 52-bit physical addreses

CRL3 -> PML4E -> Page-Directory-Pointer Table (PDPT) -> Page-Directory -> Page Table 

Initially, we disable process-context identifiers (PCIDs).