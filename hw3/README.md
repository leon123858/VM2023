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

target is let host VM and guest VM connect into same LAN

fitst build host VM
```
# start host VM
sudo bash ./run-kvm.sh -k ./linux/arch/arm64/boot/Image -i ./cloud.img
dhclient
ip tuntap add dev tap0 mode tap
ip link set dev tap0 up
ip addr add 192.168.0.101 brd + dev tap0
ip route add 192.168.0.0/24 dev tap0
```
note: should add below 2 line at the end of run-guest.sh
```
-netdev tap,id=mytap0,ifname=tap0,script=no,downscript=no,vhost=off \
-device virtio-net-pci,netdev=mytap0 \
```
second build guest VM
```
# ssh to host VM
ssh root@localhost -p 2222
# start guest VM
sudo bash ./run-guest.sh -k ./Image -i ./cloud_inner.img
dhclient
# find unused net device
ip addr
# should set enp0s3 as unused net device find by above instruction
ip addr add 192.168.0.105 brd + dev enp0s3
ip link set dev enp0s3 up
ip route add 192.168.0.0/24 dev enp0s3
```
test if is connect(can use in each VM)
```
ping 192.168.0.105
ping 192.168.0.101
```