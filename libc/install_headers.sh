#!/bin/sh
find ./include -type f | while IFS= read -r file; do
    sysroot_path=$(echo $file | cut -c 2-)
	if [ $file -nt ../sysroot/usr$sysroot_path ] || [ ! -f ../sysroot/usr$sysroot_path ]
    then
        echo "copying file $file because newer"
        mkdir -p ../sysroot/usr$(dirname $sysroot_path)
        cp -R --preserve=timestamps $file ../sysroot/usr$(dirname $sysroot_path)
    fi
done
