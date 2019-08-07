#!/bin/sh

find *.bin -type f | while IFS= read -r file; do
	if [ $file -nt ../sysroot/boot/$(basename $file) ] || [ ! -f ../sysroot/boot/$(basename $file) ]
    then
        mkdir -p ../sysroot/boot/
        echo "copying file $file because newer"
        cp -R --preserve=timestamps $file ../sysroot/boot/
    fi
done