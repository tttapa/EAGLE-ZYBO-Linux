#!/bin/sh

cd /home/develop/linux-4.4
cp $WORKSPACE_ROOT/lkm/config .config
make \
    ARCH=arm \
    CROSS_COMPILE=arm-cortexa9_neon-linux-gnueabihf- \
    oldconfig
make \
    ARCH=arm \
    CROSS_COMPILE=arm-cortexa9_neon-linux-gnueabihf- \
    -j$(nproc)