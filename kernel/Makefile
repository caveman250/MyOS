WORKINGDIR=$(shell pwd)

PREFIX = /usr
BOOTDIR = /boot
INCLUDEDIR = $(PREFIX)/include

CFLAGS = -O2 -g -ffreestanding -Wall -Wextra
CPPFLAGS = -D__is_kernel -Iinclude
LIBS = -nostdlib -lk -lgcc

SYSROOT? =
CPP = i686-elf-g++ -g -O0 --sysroot=$(SYSROOT) -isystem=$(PREFIX)/include
CC = i686-elf-gcc -g -O0 --sysroot=$(SYSROOT) -isystem=$(PREFIX)/include

ARCHDIR = arch/i386

KERNEL_OBJS=\
$(WORKINGDIR)/kernel/kernel.o\
$(WORKINGDIR)/$(ARCHDIR)/boot.asmo \
$(WORKINGDIR)/$(ARCHDIR)/kernel/Terminal.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/HAL.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/HAL.asmo \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/GDT.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/GDT.asmo \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/IDT.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/IDT.asmo \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/CPU.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/CPU.asmo \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/PIC.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/PIT.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/PIT.asmo \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/SerialPort.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/DMAManager.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/Exception.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/drivers/Keyboard.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/drivers/Keyboard.asmo \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/drivers/FloppyDisk.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/hal/drivers/FloppyDisk.asmo \
$(WORKINGDIR)/$(ARCHDIR)/kernel/MemoryManagement/PhysicalMemoryManager.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/MemoryManagement/VirtualMemoryManager.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/MemoryManagement/PageDirectoryEntry.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/MemoryManagement/PageTableEntry.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/Tests/KernelTests.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/FileSystem.o \
$(WORKINGDIR)/$(ARCHDIR)/kernel/FAT12.o \

OBJS=\
$(WORKINGDIR)/$(ARCHDIR)/crti.asmo \
$(WORKINGDIR)/$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(WORKINGDIR)/$(ARCHDIR)/crtend.o \
$(WORKINGDIR)/$(ARCHDIR)/crtn.asmo \

LINK_LIST=\
$(WORKINGDIR)/$(ARCHDIR)/crti.asmo \
$(WORKINGDIR)/$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(WORKINGDIR)/$(ARCHDIR)/crtend.o \
$(WORKINGDIR)/$(ARCHDIR)/crtn.asmo \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .cpp .asm

clean:
	rm -f myos.bin
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

myos.bin: $(OBJS) $(WORKINGDIR)/$(ARCHDIR)/linker.ld
	$(CPP) -T $(WORKINGDIR)/$(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot myos.bin

$(WORKINGDIR)/$(ARCHDIR)/crtbegin.o $(WORKINGDIR)/$(ARCHDIR)/crtend.o:
	OBJ=`$(CPP) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.cpp.o:
	$(CPP) -MD -c -masm=intel $< -o $@ $(CFLAGS) $(CPPFLAGS)

.c.o:
	$(CPP) -MD -c -masm=intel $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.asmo : %.asm
	nasm -felf32 $< -o $@

install: install-headers install-kernel

install-headers:
	sh install_headers.sh

install-kernel: myos.bin
	sh install_libs.sh

#-include $(OBJS:.asmo=.asmd)
