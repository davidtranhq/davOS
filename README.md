# davOS

davOS is an operating system written in C++. As a WIP, currently it is just a minimal kernel for x86_64 that boots with the [Limine boot protocol](https://github.com/limine-bootloader/limine/blob/trunk/PROTOCOL.md).

## Build

### Building a GCC cross-compiler

A GCC cross-compiler targetting x86_64-elf needs to be built. Follow [these instructions](docs/building-an-x86-64-gcc-cross-compiler.md) to build one.

### Building the OS

Run `make` to build the kernel `davos.elf`.

Run `make iso` to build the kernel and a bootable CD-ROM `davos.iso`.


## Booting the OS

The OS can be booted using any VM or physical machine capable of booting from CD-ROMs. For development, consider the [qemu](https://www.qemu.org/download/) VM.

If using qemu, the OS can be booted from the ISO using `qemu-system-x86_64 -cdrom davos.iso`.

Alternatively, run them `make qemu` to build the kernel and CD-ROM and run qemu all in one step.

### QEMU on WSL2

Additional steps need to be taken to run qemu on WSL2 since it requires a GUI. [https://skeptric.com/wsl2-xserver/](https://web.archive.org/web/20220617121003/https://skeptric.com/wsl2-xserver/) provides a step-by-step guide on setting up a Windows X Server for running WSl2 GUI applications using [VcXsrv](https://sourceforge.net/projects/vcxsrv/).
