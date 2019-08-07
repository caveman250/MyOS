SYSROOT=$(shell pwd)/sysroot

build: headers
	cd libc && SYSROOT=$(SYSROOT) $(MAKE) install && cd ../
	cd kernel && SYSROOT=$(SYSROOT) $(MAKE) install && cd ../

run: image
	qemu-system-i386 -cdrom myos.iso

headers:
	cd libc && SYSROOT=$(SYSROOT) $(MAKE) install-headers
	cd kernel && SYSROOT=$(SYSROOT) $(MAKE) install-headers

image: build
	sh create_iso.sh

.PHONY: clean

clean:
	cd libc && $(MAKE) clean && cd ../
	cd kernel && $(MAKE) clean && cd ../

	rm -rf sysroot
	rm -rf isodir
	rm -rf myos.iso