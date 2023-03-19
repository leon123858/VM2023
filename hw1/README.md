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
git clone --depth 1 --branch v5.15 https://github.com/torvalds/linux.git
cd linux
# can edit your own KVM here
make defconfig && make -j8
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

```
./run-kvm.sh -k ./linux/arch/arm64/boot/Image -i ./cloud.img
```
