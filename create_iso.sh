last_modified_os=`stat -c "%Y" myos.bin`
last_modified_iso=`stat -c "%Y" myos.iso`

if [ ! -f myos.iso ] || [ $last_modified_iso -lt $last_modified_os ]
then
	mkdir -p isodir/boot/grub
	cp myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir	
else
	echo "iso allready up to date"
fi
