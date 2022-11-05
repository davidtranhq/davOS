# davOS

davOS is an operating system written in C++.

## WIP

Currently, it is just a minimal kernel for 32-bit x86 that outputs text using the VGA text mode buffer.

## Build

### Building a GCC cross-compiler

A GCC cross-compiler needs to be built targetting i686-elf. The [OSDev wiki](https://wiki.osdev.org/GCC_Cross-Compiler) gives an in-depth tutorial. The GCC and binutils versions used for building the cross-compiler for this project were 11.3.0 and 2.38, respecitvely.

Aftewards, verify that the compiler was properly built and added to the path by running `i686-elf-g++ --version`.

### Building the OS

Run `build.sh` to build the kernel `kernel/davos.kernel`.

Run `iso.sh` to build the kernel and format the OS into a bootable CD-ROM `davos.iso`.

Run `clean.sh` to remove everything created from `build.sh` and `iso.sh`.
## Booting the OS

The OS can be booted using any VM or physical machine capable of booting from CD-ROMs. For development, consider the [qemu](https://www.qemu.org/download/) VM.

If using qemu, the OS can be booted from the ISO using `qemu-system-i386 -cdrom davos.iso`, or from the binary using `qemu-system-i386 -kernel kernel/davos.kernel`.

Alternatively, run them `qemu.sh` to build the kernel, `.iso`, and run qemu all in one step.

### QEMU on WSL2

Additional steps need to be taken to run qemu on WSL2 since it requires a GUI. https://skeptric.com/wsl2-xserver/ provides a step-by-step guide on setting up a Windows X Server for running WSl2 GUI applications using [VcXsrv](https://sourceforge.net/projects/vcxsrv/).


## Directory Structure

* `kernel` : all the source files used to build the kernel.
    * `arch` : architecture-specific source files
        * `i386` : implementation for the i386 (x86) architecture
    * `include/kernel` : interfaces used by the kernel, implemented for each architecture in `arch` 
    * `kernel` : architecture-agnostic source files
* `libc` : Standard C library implementation. Both freestanding and hosted versions of the library are implemented, with compiler flags used to switch between the two.


