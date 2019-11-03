WORKINGDIR=$(shell pwd)
SYSROOT=$(WORKINGDIR)/sysroot

build: headers
	cd libc && SYSROOT=$(SYSROOT) $(MAKE) install && cd ../
	cd kernel && SYSROOT=$(SYSROOT) $(MAKE) install && cd ../

run_bochs: image
	bochs -f ./bochs/bochs_config -q

run: image
	qemu-system-i386 -cdrom myos.iso -fda floppy.img -serial file:$(WORKINGDIR)/serial.log -boot d

headers:
	cd libc && SYSROOT=$(SYSROOT) $(MAKE) install-headers
	cd kernel && SYSROOT=$(SYSROOT) $(MAKE) install-headers

image: build
	sh create_iso.sh
	sh create_floppy_image.sh

.PHONY: clean

clean:
	cd libc && $(MAKE) clean && cd ../
	cd kernel && $(MAKE) clean && cd ../

	rm -rf sysroot
	rm -rf isodir
	rm -rf myos.iso
