WORKINGDIR=$(shell pwd)

PREFIX=/usr
INCLUDEDIR=$(PREFIX)/include
LIBDIR=$(PREFIX)/lib

CFLAGS = -O2 -g -ffreestanding -Wall -Wextra
CPPFLAGS = -D__is_libc -Iinclude
LIBK_CFLAGS = $(CFLAGS)
LIBK_CPPFLAGS = $(CPPFLAGS) -D__is_libk

SYSROOT?=
CPP = $(WORKINGDIR)/../cross_compiler/bin/i686-elf-g++ -g --sysroot=$(SYSROOT) -isystem=$(INCLUDEDIR)
CC = $(WORKINGDIR)/../cross_compiler/bin/i686-elf-gcc -g --sysroot=$(SYSROOT) -isystem=$(INCLUDEDIR)

ARCHDIR=arch/i386

OBJS=\
$(WORKINGDIR)/stdio/printf.o \
$(WORKINGDIR)/stdio/putchar.o \
$(WORKINGDIR)/stdio/puts.o \
$(WORKINGDIR)/stdlib/abort.o \
$(WORKINGDIR)/string/memcmp.o \
$(WORKINGDIR)/string/memcpy.o \
$(WORKINGDIR)/string/memmove.o \
$(WORKINGDIR)/string/memset.o \
$(WORKINGDIR)/string/strlen.o \
$(WORKINGDIR)/string/strcmp.o \
$(WORKINGDIR)/string/strchr.o \
$(WORKINGDIR)/string/strcpy.o \
$(WORKINGDIR)/string/strstr.o \


LIBK_OBJS=$(OBJS:.o=.libk.o)

BINARIES=libk.a

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libk.o .c

all: $(BINARIES)

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

.cpp.o:
	echo $(WORKINGDIR)
	$(CPP) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.cpp.libk.o:
	$(CPP) -MD -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

.c.o:
	$(CPP) -MD -c -masm=intel $< -o $@ $(CFLAGS) $(CPPFLAGS)

.c.libk.o:
	$(CPP) -MD -c -masm=intel $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

clean:
	rm -f $(BINARIES) *.a
	rm -f $(OBJS) $(LIBK_OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) $(LIBK_OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-libs

install-headers:
	sh install_headers.sh

install-libs: $(BINARIES)
	sh install_libs.sh

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
