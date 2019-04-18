#!/usr/bin/env bash

set -ex

dd if=/dev/zero of=sysroot.ext4 bs=1024 count=512000 # 0.5 GiB file
mkfs -t ext4 sysroot.ext4
mkdir -p /tmp/mnt-sysroot
sudo mount -t ext4 -o loop sysroot.ext4 /tmp/mnt-sysroot
sudo chown -R $USER /tmp/mnt-sysroot
sudo docker cp \
    ct-ng-devenv:/home/develop/x-tools/arm-cortexa9_neon-linux-gnueabihf/arm-cortexa9_neon-linux-gnueabihf/sysroot/. \
    /tmp/mnt-sysroot
sync -f /tmp/mnt-sysroot/*
sudo umount /tmp/mnt-sysroot