# WIP

See `vmm.cpp:add_initial_mappings`, right now we are defining the initial virtual-to-physical
mappings that we want for our operating system. I want to start by only mapping the bare minimum
of what is needed, so:

1. The kernel: mapping using the LinkerAddresses defined in `linker.ld` to the physical kernel
initialized by Limine (this includes the stack)
2. The framebuffer: use HHDM instead of identity, want to keep all virtual kernel addresses
in the HHDM

anything else?
     
