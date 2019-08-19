PREFIX=/usr
INCLUDEDIR=$(PREFIX)/include
LIBDIR=$(PREFIX)/lib

CFLAGS = -O2 -g -ffreestanding -Wall -Wextra
CPPFLAGS = -D__is_libc -Iinclude
LIBK_CFLAGS = $(CFLAGS)
LIBK_CPPFLAGS = $(CPPFLAGS) -D__is_libk

SYSROOT?=
CPP = i686-elf-g++ --sysroot=$(SYSROOT) -isystem=$(INCLUDEDIR)
CC = i686-elf-gcc --sysroot=$(SYSROOT) -isystem=$(INCLUDEDIR)

ARCHDIR=arch/i386

OBJS=\
stdio/printf.o \
stdio/putchar.o \
stdio/puts.o \
stdlib/abort.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/strlen.o \

LIBK_OBJS=$(OBJS:.o=.libk.o)

BINARIES=libk.a

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libk.o .c

all: $(BINARIES)

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

.cpp.o:
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
