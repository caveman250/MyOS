mkdir cross_compiler
cd cross_compiler
mkdir src


if VERB="$( which apt-get )" 2> /dev/null; then
	sudo apt update
        sudo apt install bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo nasm xorriso mtools qemu qemu-system gdb
elif VERB="$( which yum )" 2> /dev/null; then
        echo "Modern Red Hat-based:  unsupported, please follow the manual instructions found in the readme"
	exit 1
elif VERB="$( which portage )" 2> /dev/null; then
        echo "Gentoo-based: unsupported, please follow the manual instructions found in the readme"
	exit 1
elif VERB="$( which pacman )" 2> /dev/null; then
         sudo pacman -Sy bison flex gmp mpc mpfr texinfo nasm xorriso mtools qemu qemu-arch-extra gdb
else
        echo "Unknown OS/Distro: unsupported, please follow the manual instructions found in the readme" >&2
        exit 1
fi

curl -O https://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.gz
tar xvzf $PWD/binutils-2.34.tar.gz -C $PWD/src/ && echo "this is done now" >> log
rm -f $PWD/binutils-2.34.tar.gz

export PREFIX="$PWD"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

cd src
mkdir build-binutils
cd build-binutils
../binutils-2.34/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j24
make install -j24

cd ../../

curl -O https://ftp.gnu.org/gnu/gcc/gcc-9.3.0/gcc-9.3.0.tar.gz
tar xvzf ./gcc-9.3.0.tar.gz -C $PWD/src/ && echo "this is done now" >> log
rm -f $PWD/gcc-9.3.0.tar.gz
cd src
mkdir build-gcc
cd build-gcc
../gcc-9.3.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc -j24
make all-target-libgcc -j24
make install-gcc -j24
make install-target-libgcc -j24
