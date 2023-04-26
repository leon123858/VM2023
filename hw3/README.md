# HW3

## ENV

install dependence for compile
```
sudo apt-get install libelf-dev
sudo apt-get install dwarves
```

reinstall linux for target version
```
git clone --depth 1 --branch v5.15 https://github.com/torvalds/linux.git
cd linux
make defconfig && make -j8 # for ARM device
make menuconfig # config like below
make -j8
# x86 device 
# make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig
# make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j8
# make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j8 menuconfig
```
Go to Device Drivers -> Network device support and turn on MAC-VLAN support and MAC-
VLAN based tap driver (turn on means make it look like <*> by hitting space). Then turn on
Device Drivers -> VHOST drivers -> Host kernel accelerator for virtio net . After that,
recompile your kernel and boot the KVM host

## set VM network

