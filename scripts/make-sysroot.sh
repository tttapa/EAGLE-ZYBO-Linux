#!/usr/bin/env bash

set -ex

# dd if=/dev/zero of=sysroot.ext4 bs=1024 count=614400 # 600 MiB file
dd if=/dev/zero of=sysroot.ext4 bs=1024 count=1024000 # 1 GiB file
mkfs -t ext4 sysroot.ext4
mkdir -p /tmp/mnt-sysroot
sudo mount -t ext4 -o loop sysroot.ext4 /tmp/mnt-sysroot
sudo chown -R $USER /tmp/mnt-sysroot
sudo docker cp \
    crypto-devenv:/home/develop/x-tools/arm-cortexa9_neon-linux-gnueabihf/arm-cortexa9_neon-linux-gnueabihf/sysroot/. \
    /tmp/mnt-sysroot
sync -f /tmp/mnt-sysroot/*
sudo umount /tmp/mnt-sysroot