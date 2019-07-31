AS = nasm
AS_FLAGS = -felf32

GPP = ~/Desktop/gcc_cross_compiler/bin/i686-elf-g++
GPP_COMPILE_FLAGS = -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
GPP_LINK_FLAGS = -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

image: link
	sh ./create_iso.sh

link: kernel.o boot.o
	$(GPP) -T linker.ld -o myos.bin $(GPP_LINK_FLAGS)

run: image
	qemu-system-i386 -cdrom myos.iso

.PHONY: clean

clean:
	rm -f ./*.bin
	rm -f ./*.o
	rm -f ./*.iso
	rm -f -R ./isodir

%.o: %.cpp
	$(GPP) -c $< -o $@ $(GPP_COMPILE_FLAGS)
	
%.o: %.asm
	$(AS) $(AS_FLAGS) $< -o $@