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

## install benchmarks for KVM
in kvm host
```
git clone https://github.com/chazy/kvmperf.git
scp -P 2222 -r ./kvmperf/ root@localhost:/root/kvmperf/
```

### test hack bench
```
wget https://raw.githubusercontent.com/linux-test-project/ltp/master/testcases/kernel/sched/cfs-scheduler/hackbench.c
gcc hackbench.c -o hackbench -lpthread
# you can adjust how many times you want to run the benchmark (how many times you run ./hackbench)
./hackbench 50 process 50
```

### test kern bench
```
sudo apt install make flex bison
wget http://ck.kolivas.org/apps/kernbench/kernbench-0.50/kernbench
sudo chmod +x kernbench
wget https://www.kernel.org/pub/linux/kernel/v5.x/linux-5.15.tar.gz
tar xfvz linux-5.15.tar.gz
cd linux-5.15
# the -n option specifies how many runs you would like to perform (default = 5)
../kernbench -M -H -n 1 | tee >(grep 'Elapsed' | awk '{print $3 }' >> kernbench.txt)
```

### test netperf

note: need to set IP before

```
# install tool in each VM (guest and host)
wget http://ports.ubuntu.com/pool/multiverse/n/netperf/netperf_2.6.0-2.1_arm64.deb
sudo dpkg -i netperf_2.6.0-2.1_arm64.deb
# run in server (host VM)
netserver -p 4444
# run in client (guest VM)
vim ./kvmperf/cmdline_tests/netperf.sh
# -for _TEST in TCP_MAERTS TCP_STREAM TCP_RR; do
# +for _TEST in TCP_STREAM TCP_RR; do
# can use `ip addr show` check your IP
sudo bash ./kvmperf/cmdline_tests/netperf.sh 192.168.0.101
```

### test apache

note: need to set IP before

```
# run in server
sudo bash ./kvmperf/cmdline_tests/apache_install.sh
# run in client
sudo bash ./kvmperf/cmdline_tests/apache_install.sh
# can use `ip addr show` check your IP
sudo bash ./kvmperf/cmdline_tests/apache.sh 192.168.0.101 1 # server ip = 192.168.0.101, test 1 times
```

### performance tool install
only need in host KVM
```
sudo apt-get install linux-tools-common linux-tools-generic
rm /usr/bin/perf
sudo ln -s /usr/lib/linux-tools/5.4.0-148-generic/perf /usr/bin/per # version of perf maybe will different
# example command
perf kvm --host top
```
