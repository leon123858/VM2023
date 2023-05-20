# HW4 doc

## Part 1

please check docker/

## Part 2

### 執行

1. 引用 patch 於 hw1 中的 qemu version
2. 利用 run-kvm.sh 執行
3. 利用 root 登入, 可以任意 password

### 重要概念

kvm host 是使用 tcg (tiny code generate) 機制實作, 會以 block 即時編譯的模式進行, 所以核心概念是在編譯階段把 pam 的方法的回傳值都設為成功(0)

`gen_intermediate_code` 是關鍵, 即時編譯在 `tb_gen_code` 此時會分兩個 stage
即:

- frontend: source code to intermediate code
- backend: intermediate code to target code

我們要做的是在 frontend 偷偷改寫程式碼

### 環境設置

利用 hw1 的方法建置一個新的 KVM host image, 但不要設置密碼

```
wget https://cloud-images.ubuntu.com/releases/focal/release/ubuntu-20.04-server-cloudimg-arm64-root.tar.xz
qemu-img create -f raw cloud.img 25g
mkfs.ext4 cloud.img
mount cloud.img /mnt
tar xvf ./ubuntu-20.04-server-cloudimg-arm64-root.tar.xz -C /mnt
sync
touch /mnt/etc/cloud/cloud-init.disabled
# 可以在這裡檢查 "執行檔是否和助教符合"
umount /mnt
```

檢視執行檔是否和助教符合

```
# my
root@ubuntu:/bin# md5sum login
aeada2a4fdf5c3f62eb169c0617030d7  login
# ta
# md5sum login
aeada2a4fdf5c3f62eb169c0617030d7 login
```

### 工具

#### 在 qemu 裏面 log

```
# 於 kvm-run.sh 加上 `-D File` 參數如下
qemu-system-aarch64 -nographic -machine virt,gic-version=2 -m ${MEMSIZE} -cpu cortex-a57 -smp ${SMP} -machine virtualization=on \
        -kernel ${KERNEL} ${DTB} \
        -drive if=none,file=$FS,id=vda,cache=none,format=raw \
        -device virtio-blk-pci,drive=vda \
        -display none \
        -serial $CONSOLE \
        -append "console=ttyAMA0 root=/dev/vda rw $CMDLINE" \
        -netdev user,id=net0,hostfwd=tcp::2222-:22 \
        -device virtio-net-pci,netdev=net0,mac=de:ad:be:ef:41:49 \
        -D ./log.log \
```

之後在 qemu 程式碼中可以利用 `qemu_log("...",...)` 來 log 相關資訊協助開發

#### 利用 objdump 反編譯

```
objdump -d login > ./login.log
```
