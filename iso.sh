#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/davos.kernel isodir/boot/davos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "davos" {
	multiboot /boot/davos.kernel
}
EOF
grub-mkrescue -o davos.iso isodir
