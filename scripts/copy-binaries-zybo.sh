#!/usr/bin/env bash

echo "Copying files from \`$1\` to \`root@drone:$2\`"

cd $1
for file in *
do
    remote_mtime=`ssh root@drone date -ur "$2/$file" +%s 2> /dev/null` \
     || remote_mtime=0
    local_mtime=`date -ur $file +%s`
    if ((local_mtime > remote_mtime)); then
        echo "copying $file"
        scp -p $file "root@drone:$2/$file"
    else
        echo "$file is up to date"
    fi 
done