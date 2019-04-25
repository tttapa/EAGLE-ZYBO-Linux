#!/usr/bin/env bash

cd $1/build-arm/bin
for file in *
do
    remote_mtime=`ssh root@drone date -ur /media/$file +%s`
    local_mtime=`date -ur $file +%s`
    if ((local_mtime > remote_mtime)); then
        scp -p $file root@drone:/media/$file
    else
        echo $file is up to date
    fi 
done