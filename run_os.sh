#!/bin/sh

DEBUG=0

while [ $# -gt 0 ]
do
key="$1"

case $key in
    --debug)
    DEBUG=1
    shift # past argument
    ;;
esac
done

if grep -q Microsoft /proc/version; then
  wsl_display=:0
  echo "WSL detected, exporting display as $wsl_display, see the 'wsl_display' variable in run_os.sh to change. "
  export DISPLAY=$wsl_display
fi

if [ $DEBUG = 1 ]; then
    echo finish #this is just here so VSCode wont wait for qemu to exit before it starts debugging.
    qemu-system-i386 -S -s -cdrom myos.iso -fda floppy.img -serial file:./serial.log -boot d
else
    qemu-system-i386 -cdrom myos.iso -fda floppy.img -serial file:./serial.log -boot d
fi


