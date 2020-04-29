#!/bin/bash
sudo dmesg -C
for file in OS_PJ1_Test/*; do
    name=`expr "$file" : 'OS_PJ1_Test/\(.*\)\.txt'`
    sudo ./main < $file > output/${name}_stdout.txt
    dmesg | grep Project1 > output/${name}_dmesg.txt
    sudo dmesg -C
done
