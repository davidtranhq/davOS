#! /bin/sh

if grub-file --is-x86-multiboot $1; then
    echo multiboot 1 confirmed
else
    echo the file is not multiboot 1
fi