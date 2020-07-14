# Kernel Development
## Linux Kernel Compilation
[kernel v4.14.25](https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.14.25.tar.xz)
[kernel website](http://www.kernel.org)
[NTU mirror](http://ftp.ntu.edu.tw/linux/kernel/v4.x/)

```sh
# Setting up build environment
sudo apt-get update
sudo apt-get install gcc vim ssh wget libncurses5-dev libssl-dev
# Getting the source archive
cd ${HOME}
wget https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.14.25.tar.xz
# Extracting the archive
tar Jxvf linux-4.14.25.tar.xz
cd linux-4.14.25
```
### 產生 .config
```sh
make config #（不建議）
make menuconfig #（使用 ncurses 做使用者介面）
make xconfig #（用 qt 做使用者介面）

# 你也可以直接複製自己系統裡 /boot 中的設定檔.
cp /boot/config-$(uname -r) .config
make olddefconfig # (檢查新舊版設定檔的相容性)
make menuconfig #（若想基於該設定檔繼續修改）
```

### Kernel Compilation
The first time (在助教  i5-7200u 的筆電跑了 46 分鐘):
```sh
make bzImage # (編譯核心並壓縮)
make modules # (編譯各個核心模組)
sudo make modules_install # (安裝模組)
sudo make install #（安裝核心）
sudo mkinitramfs -o /boot/initrd.img-4.14.25 # Do this whenever you reinstall the modules.
reboot
```
Others (about 5~10min):
```sh
make bzImage
sudo make install
reboot
```

press shift in grub menu, select "Advanced options for Ubuntu"

You can use `uname -a` to check current system information.

## System Call Development
kernel/my_add.c
```c
#include <linux/linkage.h>
#include <linux/kernel.h>
asmlinkage int sys_my_add(int a, int b)
{
    printk("my_add is invoked!\n");
    return a + b;
}
```

kernel/Makefile
```
obj-y += my_add.o
```

include/linux/syscalls.h
```
asmlinkage int sys_my_add(int a, int b);
```

arch/x86/entry/syscalls/syscall_64.tbl
```
333	common		my_add	sys_my_add
```
### System Call Invocation
ap.c
```c
#include <stdio.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, char** argv)
{
    printf("%d\n", syscall(333, 1, 1));
}
```
```sh
gcc -o ap ap.c
./ap
```

## Project 1

https://stackoverflow.com/questions/50349294/getnstimeofday-is-an-implicit-declaration-in-system-call-when-linux-time-h-i
