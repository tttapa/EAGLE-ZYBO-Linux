#!/bin/sh

cd /home/develop/linux-4.4.1
cp $WORKSPACE_ROOT/lkm/config .config
make \
    ARCH=arm \
    CROSS_COMPILE=/home/develop/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf- \
    oldconfig
make \
    ARCH=arm \
    CROSS_COMPILE=/home/develop/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf- \
    -j$(nproc)