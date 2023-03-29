# HW2

## 常用指令筆記

登入 VM
```
ssh root@localhost -p 2222
```

把 ioctl client 移入虛擬機
```
scp -P 2222 ./client/client  root@localhost:/root
```

執行 VM
```
# kvm host
sudo bash ./run-kvm.sh -k ./linux/arch/arm64/boot/Image -i ./cloud.img
# guest vm
sudo bash ./run-guest.sh -k ./Image -i ./cloud_inner.img
```
