# Hw1

## init env

suppose you are run in ARMv8 VM

### install tool

note: should start container

```
# 安裝相依工具
sudo bash ./install_tools.sh
```

### install qemu

```
git clone https://gitlab.com/qemu-project/qemu.git
cd qemu/
git checkout tags/v7.0.0
./configure --target-list=aarch64-softmmu --disable-werror
make -j8 && make install
```

### install KVM

```
sudo apt-get install gcc-aarch64-linux-gnu
git clone --depth 1 --branch v5.15 https://github.com/torvalds/linux.git
cd linux
# can edit your own KVM here
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j8
```

### Create virtual disk image

```
wget https://cloud-images.ubuntu.com/releases/focal/release/ubuntu-20.04-server-cloudimg-arm64-root.tar.xz
qemu-img create -f raw cloud.img 25g
mkfs.ext4 cloud.img
mount cloud.img /mnt
tar xvf ./ubuntu-20.04-server-cloudimg-arm64-root.tar.xz -C /mnt
sync
touch /mnt/etc/cloud/cloud-init.disabled
# edit first line to `root::0:0:root:/root:/bin/bash`
vim /mnt/etc/passwd
umount /mnt
```

## Run KVM host
note: 只要看到有 login 出來, 就可以直接輸入帳號 `root` login 了
```
./run-kvm.sh -k ./linux/arch/arm64/boot/Image -i ./cloud.img
```

## set KVM host ssh

登入後直接操作

```
# 更新 ssh server 設置
dpkg-reconfigure openssh-server
# 要完成 ssh server 設置更改(內容後述)
sudo vim  /etc/ssh/sshd_config
# 更新 ssh server 設置
dpkg-reconfigure openssh-server
```

sshd_config update item

```
# 允許管理者登入
PermitRootLogin yes
# 關閉公鑰登入
PubkeyAuthentication no
# 啟動密碼登入
PasswordAuthentication yes
# 允許無密碼
PermitEmptyPasswords yes
```

設置完成後可以用以下指令直接登入

new KVM host should use `dhclient` init ssh IP

each ssh client use `ssh root@localhost -p 2222` to connect

## 常用指令筆記

清除殭屍虛擬機
```
sudo netstat -lpn |grep 2222
sudo kill -9 pid
```

part2
```
// open ssh
dhclient
// move new KVM
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j8
scp -P 2222 ./myKVM/linux/arch/arm64/boot/Image root@localhost:/root
// run guest
ssh root@localhost -p 2222
sudo bash ./run-guest.sh -k ./Image  -i ./cloud_inner.img
```
