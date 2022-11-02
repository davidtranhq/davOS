# davOS

davOS is an operating system written in C++.

## WIP

Currently, it is just a minimal kernel for 32-bit x86 that outputs text using the VGA text mode buffer.

## Build

### Building a GCC cross-compiler

A GCC cross-compiler needs to be built targetting i686-elf. The [OSDev wiki](https://wiki.osdev.org/GCC_Cross-Compiler) gives an in-depth tutorial. The GCC and binutils versions used for building the cross-compiler for this project were 11.3.0 and 2.38, respecitvely.

Aftewards, verify that the compiler was properly built and added to the path by running `i686-elf-g++ --version`.

Note: because of a [long-standing GCC bug](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100057), building C++ binaries with a freestanding C++ Standard Library implementation doesn't work properly with GCC. It is possible that this will be fixed in [GCC version 13](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2268r0.html). For now, the project uses [cxxshim](https://github.com/managarm/cxxshim), a 3rd-party freestanding implementation of the C++ standard library. The `Makefile` handles these 3rd-party includes automatically.

### Building the OS

Clone the repository with the `--recurse-submodules` flag (to additionally clone the linked submodules).

Run `make` in the root directory to build both the `davos.bin` binary and the bootable `davos.iso` CD-ROM.

## Booting the OS

The OS can be booted using any VM or physical machine capable of booting from CD-ROMs. For development, consider the [qemu](https://www.qemu.org/download/) VM.

If using qemu, the OS can be booted from the ISO using `qemu-system-i386 -cdrom davos.iso`, or from the binary using `qemu-system-i386 -kernel davos.bin`.

### QEMU on WSL2

Additional steps need to be taken to run qemu on WSL2 since it requires a GUI. https://skeptric.com/wsl2-xserver/ provides a step-by-step guide on setting up a Windows X Server for running WSl2 GUI applications using [VcXsrv](https://sourceforge.net/projects/vcxsrv/).


## Directory Structure

* `build/` contains intermediate build files; generated after running `make`.
* `config/` contains configuration files for custom linking (`linker.ld`) and creating the bootable CD-ROM image (`grub.cfg`).
* `cxxshim/` is a Git submodule pointing to a 3rd party [freestanding implementation of the C++ standard library](https://en.cppreference.com/w/cpp/freestanding).
* `iso/` contains intermediate files used to generate the final `.iso` bootable CD-ROM image.
* `scripts/` contain test scripts that are run during the build process to ensure correct compilation and linking.
* `src/` contains the source C++ and assembly files.



