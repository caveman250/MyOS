#!/bin/sh
echo finish #this is just here so VSCode wont wait for qemu to exit before it starts debugging.
qemu-system-i386 -S -s -cdrom myos.iso -fda floppy.img -serial file:./serial.log -boot d