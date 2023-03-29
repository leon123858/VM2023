# HW2
## part2-1

被篡改的客戶端
```c
// sheep.c
int main() {
        while (1) {}
}
```

Makefile
```
CC      = gcc
CFLAGS  = -g
RM      = rm -f


default: all

all: Sheep

Sheep: sheep.c
        $(CC) $(CFLAGS) -o sheep sheep.c
```

process:
1. create in guest VM
2. compile and run sheep

in host VM

1. use page-types find target MEM
2. execute client/client to edit mem by new ioctl

## 獲取 guest VM address

use in vm host

```
git clone --depth 1 --branch v5.15 https://github.com/torvalds/linux.git
cd linux
# their will be a tool called page-types in `linux/tools/vm`
make -C ./tools/vm/
mv ./tools/vm/page-types ../page-types
```

## 常用指令筆記

登入 VM
```
ssh root@localhost -p 2222
```

把 ioctl client 移入虛擬機
```
scp -P 2222 -r ./client root@localhost:/root
```

執行 VM
```
# kvm host
sudo bash ./run-kvm.sh -k ./linux/arch/arm64/boot/Image -i ./cloud.img
# guest vm
sudo bash ./run-guest.sh -k ./Image -i ./cloud_inner.img
```

看 log
```
dmesg | grep -i kvm
```

導出 patch
```
git diff > hw2.patch
```

find all mappings from virtual page number to physical frame number of a process using (gpa)
```
pidof <process name>
./page-types -p <pid> -L
```
