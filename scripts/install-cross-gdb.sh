#!/usr/bin/env bash

set -ex

cd /tmp 
wget https://ftp.gnu.org/gnu/gdb/gdb-8.2.tar.xz
tar xf gdb-8.2.tar.xz
mkdir -p gdb-8.2/build
cd gdb-8.2/build
../configure --prefix=$HOME/.local --target=arm-linux-gnueabihf
make -j$(nproc)
make install