#!/bin/sh

# This scripts uses the original config file of the ZYBO Linux image to compile
# the parts of the kernel necessary to build kernel modules.
# The config file can be found in the root filesystem of the ZYBO in 
# /proc/config.gz

cd /home/develop/linux-xlnx
cp $WORKSPACE_ROOT/lkm/config .config
make \
    ARCH=arm \
    CROSS_COMPILE=arm-cortexa9_neon-linux-gnueabihf- \
    CFLAGS=-mcpu=cortex-a9 \
    oldconfig
make \
    ARCH=arm \
    CROSS_COMPILE=arm-cortexa9_neon-linux-gnueabihf- \
    CFLAGS=-mcpu=cortex-a9 \
    prepare 
# make \
#     ARCH=arm \
#     CROSS_COMPILE=arm-cortexa9_neon-linux-gnueabihf- \
#     CFLAGS=-mcpu=cortex-a9 \
#     modules_prepare 
make \
    ARCH=arm \
    CROSS_COMPILE=arm-cortexa9_neon-linux-gnueabihf- \
    CFLAGS=-mcpu=cortex-a9 \
    headers_install
# make \
#     ARCH=arm \
#     CROSS_COMPILE=arm-cortexa9_neon-linux-gnueabihf- \
#     CFLAGS=-mcpu=cortex-a9 \
#     modules

# make \
#     ARCH=arm \
#     CROSS_COMPILE=arm-cortexa9_neon-linux-gnueabihf- \
#     CFLAGS=-mcpu=cortex-a9 \
#     uImage -j$(nproc)