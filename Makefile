WORKINGDIR=$(shell pwd)
SYSROOT=$(WORKINGDIR)/sysroot

build: headers
	cd libc && SYSROOT=$(SYSROOT) $(MAKE) install && cd ../
	cd kernel && SYSROOT=$(SYSROOT) $(MAKE) install && cd ../

run_bochs: image
	bochs -f ./bochs/bochs_config -q

run: image
	sh run_os.sh

headers:
	echo start
	cd libc && SYSROOT=$(SYSROOT) $(MAKE) install-headers
	cd kernel && SYSROOT=$(SYSROOT) $(MAKE) install-headers

image: build
	sh create_iso.sh
	sh create_floppy_image.sh

count_lines:
	find . -name '*.c' -o -name '*.cpp' -o -name '*.asm' -o -name '*.sh' -o -name '*.h' -o -name 'Makefile' | xargs wc -l

.PHONY: clean

clean:
	cd libc && $(MAKE) clean && cd ../
	cd kernel && $(MAKE) clean && cd ../

	rm -rf sysroot
	rm -rf isodir
	rm -rf myos.iso
