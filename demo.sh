#!/bin/bash
sudo dmesg -C
demos=(TIME_MEASUREMENT FIFO_1 PSJF_2 RR_3 SJF_4)
for name in "${demos[@]}"; do
    echo "sudo ./main < OS_PJ1_Test/${name}.txt > demo/${name}_stdout.txt"
    sudo ./main < OS_PJ1_Test/${name}.txt > demo/${name}_stdout.txt
    echo "dmesg | grep Project1 > demo/${name}_dmesg.txt"
    dmesg | grep Project1 > demo/${name}_dmesg.txt
    echo "sudo dmesg -C"
    sudo dmesg -C
done
