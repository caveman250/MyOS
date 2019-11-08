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
kernel/kernel.o\
$(ARCHDIR)/boot.asmo \
$(ARCHDIR)/kernel/Terminal.o \
$(ARCHDIR)/kernel/hal/HAL.o \
$(ARCHDIR)/kernel/hal/HAL.asmo \
$(ARCHDIR)/kernel/hal/GDT.o \
$(ARCHDIR)/kernel/hal/GDT.asmo \
$(ARCHDIR)/kernel/hal/IDT.o \
$(ARCHDIR)/kernel/hal/IDT.asmo \
$(ARCHDIR)/kernel/hal/CPU.o \
$(ARCHDIR)/kernel/hal/CPU.asmo \
$(ARCHDIR)/kernel/hal/PIC.o \
$(ARCHDIR)/kernel/hal/PIT.o \
$(ARCHDIR)/kernel/hal/PIT.asmo \
$(ARCHDIR)/kernel/hal/SerialPort.o \
$(ARCHDIR)/kernel/hal/DMAManager.o \
$(ARCHDIR)/kernel/Exception.o \
$(ARCHDIR)/kernel/hal/drivers/Keyboard.o \
$(ARCHDIR)/kernel/hal/drivers/Keyboard.asmo \
$(ARCHDIR)/kernel/hal/drivers/FloppyDisk.o \
$(ARCHDIR)/kernel/hal/drivers/FloppyDisk.asmo \
$(ARCHDIR)/kernel/MemoryManagement/PhysicalMemoryManager.o \
$(ARCHDIR)/kernel/MemoryManagement/VirtualMemoryManager.o \
$(ARCHDIR)/kernel/MemoryManagement/PageDirectoryEntry.o \
$(ARCHDIR)/kernel/MemoryManagement/PageTableEntry.o \
$(ARCHDIR)/kernel/Tests/KernelTests.o \
$(ARCHDIR)/kernel/FileSystem.o \
$(ARCHDIR)/kernel/FAT12.o \

OBJS=\
$(ARCHDIR)/crti.asmo \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.asmo \

LINK_LIST=\
$(ARCHDIR)/crti.asmo \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.asmo \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .cpp .asm

clean:
	rm -f myos.bin
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

myos.bin: $(OBJS) $(ARCHDIR)/linker.ld
	$(CPP) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot myos.bin

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
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
