PREFIX = /usr
BOOTDIR = /boot
INCLUDEDIR = $(PREFIX)/include

CFLAGS = -O2 -g -ffreestanding -Wall -Wextra
CPPFLAGS = -D__is_kernel -Iinclude
LIBS = -nostdlib -lk -lgcc

SYSROOT? =
CPP = i686-elf-g++ --sysroot=$(SYSROOT) -isystem=$(PREFIX)/include
CC = i686-elf-gcc --sysroot=$(SYSROOT) -isystem=$(PREFIX)/include

ARCHDIR = arch/i386

KERNEL_OBJS = kernel/kernel.o $(ARCHDIR)/boot.o $(ARCHDIR)/Terminal.o

OBJS=\
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

LINK_LIST=\
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .cpp .asm

myos.bin: $(OBJS) $(ARCHDIR)/linker.ld
	$(CPP) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot myos.bin

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CPP) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.cpp.o:
	$(CPP) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.c.o:
	$(CPP) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.asm.o:
	nasm -felf32 $< -o $@

clean:
	rm -f myos.bin
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

install-headers:
	sh install_headers.sh

install-kernel: myos.bin
	sh install_libs.sh

-include $(OBJS:.o=.d)