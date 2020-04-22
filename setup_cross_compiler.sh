mkdir cross_compiler
cd cross_compiler
mkdir src

sudo pacman -S bison flex gmp mpc mpfr texinfo nasm xorriso mtools qemu qemu-arch-extra

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
make
make install

cd ../../

curl -O https://ftp.gnu.org/gnu/gcc/gcc-9.3.0/gcc-9.3.0.tar.gz
tar xvzf ./gcc-9.3.0.tar.gz -C $PWD/src/ && echo "this is done now" >> log
rm -f $PWD/gcc-9.3.0.tar.gz
cd src
mkdir build-gcc
cd build-gcc
../gcc-9.3.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
