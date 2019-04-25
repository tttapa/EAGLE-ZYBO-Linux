#!/usr/bin/env bash

set -ex

cd /tmp
wget http://downloads.sourceforge.net/project/zbar/zbar/0.10/zbar-0.10.tar.bz2
tar xjf zbar-0.10.tar.bz2
rm zbar-0.10.tar.bz2

mkdir -p zbar-0.10/build
cd zbar-0.10/build
../configure \
    --disable-video --without-imagemagick --without-gtk --without-python \
    --without-qt --without-jpeg \
    CFLAGS= \
    --prefix=$HOME/.local
make -j$(nproc)
make install

cd /tmp
mkdir -p zbar-0.10/build-arm
cd zbar-0.10/build-arm
../configure \
    --disable-video --without-imagemagick --without-gtk --without-python \
    --without-qt --without-jpeg \
    CFLAGS= \
    CC=arm-cortexa9_neon-linux-gnueabihf-gcc \
    --host=arm-cortexa9_neon-linux-gnueabihf \
    --prefix=$TOOLCHAIN_PATH/arm-cortexa9_neon-linux-gnueabihf/sysroot
make -j$(nproc)
make install