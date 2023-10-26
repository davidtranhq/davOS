# WIP

We are triple faulting after trying to load the newly created page table into the PTBR.
Either the page table data isn't initialized correctly (likely), we aren't loading the
correct physical/virtual address of the page table into the PTBR, or something else.