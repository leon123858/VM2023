# Hw1

## init env

suppose you are run in ARMv8 VM

### set UTM VM
```
# install live server
# after reboot in first execute, 會發現卡在黑畫面, 退出磁碟, 接著關機, 再重開
# 利用官方教學安裝 desktop
```

進入 VM 後
```
# 安裝軟件管理
sudo apt install synaptic
# 利用 GUI 安裝 firefox
# 利用 firefox 下載 vscode
# 安裝 vscode
sudo dpkg -i <vscode name>.deb
# 下載 gh
# 移除 警告 for qeum
sudo apt-get purge needrestart
```

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
# ARM 主機上不用安裝下一行
sudo apt-get install gcc-aarch64-linux-gnu
git clone --depth 1 --branch v5.15 https://github.com/torvalds/linux.git
cd linux
# can edit your own KVM here
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j8
```

note: arm 主機上末兩行可以改為 `make defconfig && make -j8`

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

## set inside VM

在內部 VM (host VM)

```
# 創建安裝相依腳本 內容複製 ./install_tools.sh
vim install.sh
# 安裝
sudo bash install.sh
# 安裝 qemu (可用 `apt install qemu-system-arm qemu` 取代, 但可能有 bug)
git clone https://gitlab.com/qemu-project/qemu.git
cd qemu/
git checkout tags/v7.0.0
./configure --target-list=aarch64-softmmu --disable-werror
make -j8
sudo make install
# create inner VM disk
qemu-img create -f raw cloud_inner.img 2g
mkfs.ext4 cloud_inner.img
sudo mount cloud_inner.img /mnt
sudo tar xvf ./ubuntu-20.04-server-cloudimg-arm64-root.tar.xz -C /mnt
sync
sudo touch /mnt/etc/cloud/cloud-init.disabled
# 編輯第一行, 把 root 設為沒密碼同上一階段行為
sudo vim /mnt/etc/passwd
sudo umount /mnt
# 把 virtual 硬碟放進 VM
scp -P 2222 cloud_inner.img root@localhost:/root
# 把 KVM 放進 VM
scp -P 2222 ./linux/arch/arm64/boot/Image root@localhost:/root
# 把跑 guest 腳本放入 guest VM
scp -P 2222 $PATH_TO_FILES/run-guest.sh root@localhost:/root
# 執行 guest VM
./run-guest.sh -k ./Image -i ./cloud_inner.img
```

## 常用指令筆記

清除殭屍虛擬機
```
sudo netstat -lpn |grep 2222
sudo kill -9 pid
```

看 printk
```
dmesg | grep -i kvm
```

## part 2 作業訣竅

要改的虛擬機是佈置在 ubuntu linux 裡面的 VM, 不是最中間的 VM,
因為最中間的 VM 啟動不完整, 不會觸發 content switch,
要改的 code 是在 restore general reg 時檢測特定的 pattern, 
再強制變更目標 reg , 過程皆需使用 ARM 組合語言
