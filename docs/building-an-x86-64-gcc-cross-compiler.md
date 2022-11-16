# Building an x86_64 Cross-Compiler

The [OSDev wiki](https://wiki.osdev.org/Libgcc_without_red_zone) gives an in-depth tutorial. The modifications listed under https://wiki.osdev.org/Libgcc_without_red_zone need to be followed as well to support x86_64.

## Instructions

The articles in the above links are detailed enough to follow, but the steps are also listed below here for posterity. The following steps were tested on Ubuntu using GCC version 11.3.0 and binutils version 2.38, the versions that come pre-installed on Ubuntu 22.04.1 LTS.

1. Create a build source directory (e.g. `$HOME/gcc-cross`) to hold the GCC source files.
1. Download and extract the [GCC source files](https://ftp.gnu.org/gnu/gcc/) corresponding to your installed GCC version (use `gcc --version` to check) to the build source directory.
1. Download and extract the [binutils source files](https://ftp.gnu.org/gnu/binutils/?C=M;O=D) corresponding to your installed binutils version (use `ld --version` to check) to the build source directory.
1. Install the dependencies listed [here](https://wiki.osdev.org/GCC_Cross-Compiler#Installing_Dependencies) using the following command:
    ```
    sudo apt install bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
    ```
1. Export the following environnment variables (configured to your liking)
    ```bash
    export PREFIX="$HOME/opt/cross" # directory to install the compiler (under $PREFIX/bin)
    export TARGET=x86_64-elf
    export PATH="$PREFIX/bin:$PATH" 
    ```
1. Build `binutils`:
    ```bash
    cd $HOME/gcc-cross # your build source directory
 
    mkdir build-binutils
    cd build-binutils
    ../binutils-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror # replace x.y.z with your binutils version
    make
    make install # this may require a sudo if the install directory $PREFIX is protected
    ```
1. Run the following to check that `binutils` was properly installed from the above steps:
    ```bash
    # was the assembler built and installed at the expected path?
    which -- $TARGET-as || echo $TARGET-as is not in the PATH
    ```
1. Modify the GCC source files to exclude redzone from `libgcc`:
    1. Create a file with the following contents under `gcc-x.y.z/gcc/config/i386/t-x86_64-elf` (where `gcc-x.y.z` are the downloaded GCC sources)
        ```bash
        # Add libgcc multilib variant without red-zone requirement
    
        MULTILIB_OPTIONS += mno-red-zone
        MULTILIB_DIRNAMES += no-red-zone
        ```
    1. Modify the x86_64-elf configuration located in the `gcc-x.y.z/gcc/config.gcc` file:
        ```bash
         x86_64-*-elf*)
            tmake_file="${tmake_file} i386/t-x86_64-elf" # <-- add this line
            tm_file="${tm_file} i386/unix.h i386/att.h dbxelf.h elfos.h newlib-stdint.h i386/i386elf.h i386/x86-64.h"
            ;;
        ```
1. Build `gcc`:
    ```bash
    cd $HOME/gcc-cross # your build source directory

    mkdir build-gcc
    cd build-gcc
    ../gcc-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers # replace x.y.z with your binutils version
    make all-gcc
    make all-target-libgcc
    make install-gcc
    make install-target-libgcc
    ```
1. Append the following to `~/.profile` to add the built compilers and linkers to the path:
    ```bash
        export PATH="<PREFIX>/bin:$PATH"
    ```
    where `<PREFIX>` is the value of the chosen install directory (the value of `$PREFIX`).

### Verification

You can verify that both the C and C++ compiilers were properly built with
```bash
$PREFIX/bin/$TARGET-gcc --version
```
and
```bash
$PREFIX/bin/$TARGET-g++ --version
```

Check also that there are two versions of `libgcc` (one with red-zone and one without) with
```bash
find $PREFIX/lib -name 'libgcc.a'
```
which should output
```
./gcc/x86_64-pc-elf/x.y.z/libgcc.a
./gcc/x86_64-pc-elf/x.y.z/no-red-zone/libgcc.a
```


