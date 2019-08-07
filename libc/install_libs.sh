#!/bin/sh

find *.a -type f | while IFS= read -r file; do
	if [ $file -nt ../sysroot/usr/lib/$(basename $file) ] || [ ! -f ../sysroot/usr/lib/$(basename $file) ]
    then
        mkdir -p ../sysroot/usr/lib
        echo "copying file $file because newer"
        cp -R --preserve=timestamps $file ../sysroot/usr/lib/
    fi
done