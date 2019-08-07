iso_out_of_date=false

if [ ! -f "myos.iso" >&- ]
then
	echo "iso doesnt exist, building."
	iso_out_of_date=true
fi

files=$(find sysroot/usr/lib/* -type f)
for file in $files;
do
	if [ $file -nt "myos.iso" ]
    then
		echo "$file has been updated since the last iso, marking iso out of date"
        iso_out_of_date=true
    fi
done

files=$(find sysroot/boot/* -type f)
for file in $files;
do
	echo $file
	if [ $file -nt "myos.iso" ]
    then
		echo "$file has been updated since the last iso, marking iso out of date"
        iso_out_of_date=true
    fi
done

if [ "$iso_out_of_date" = true ]
then
	echo "building iso"
	mkdir -p isodir/boot/grub
	cp sysroot/boot/myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir	
else
	echo "iso allready up to date"
fi