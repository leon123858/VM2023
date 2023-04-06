# HW2

## 作業 SPEC

詳見: [homework2](./docs/vmhw2.pdf)

## Part.1 (Adding new ioctls to KVM)

下載 linux v5.15 後, 使用補丁編譯後即可使用

```
git clone --depth 1 --branch v5.15 https://github.com/torvalds/linux.git
cd linux
git apply <path of hw2.patch>
# compile for your platform (skip)
```

代碼說明

我把此部分作業的程式碼拆分成 3 大區塊來做說明

1. ioctl (新增 API 介面)

   ```
   struct kvm_arm_write_gpa_args {
       uint32_t vmid;  // the vmid that you, as the host, want to write to
       uint64_t gpa;   // the gpa of the guest
       uint64_t data;  // address of the payload in host user space
       uint64_t size;  // size of the payload
       };

        #define KVM_ARM_WRITE_GPA  _IOW(KVMIO, 0xfe, struct kvm_arm_write_gpa_args)
   ```

   在對應資料夾定義新的 ioctl api 介面

   在 `kvm_dev_ioctl` 加入新的 case 處理此新 API
   如此一來這個 API 就會是 system api, 可以在 user space 被成功呼叫

   接著透過自行撰寫的 `get_vm_by_id` 來取得目標 vm 地址

2. 要求中斷與傳遞參數

   ```
   vm_class_target_vcpu = target_vm->vcpus[0];
   vm_class_is_trigger = 1;
   vm_class_gpa = gpa_para_struct->gpa;
   vm_class_size = gpa_para_struct->size;
   memcpy(vm_class_data, gpa_para_struct->data, vm_class_size);
   kvm_make_request(KVM_REQ_IRQ_PENDING, vm_class_target_vcpu);
   kvm_vcpu_kick(vm_class_target_vcpu);
   ```

   需要利用 extern 在 global 創建共用 buffer 來使不同 ioctl 間傳遞參數, 所以取出參數後要放入 global extern variable 中
   值得注意的是, 客戶端的數據要複製到共用 buffer, 因為 ioctl 取得的 data pointer 是客戶端自己的 VA, 另一個 ioctl 的執行者 (qemu) 無法取得數據

   `kvm_make_request` 的作用是把中斷請求放入 vcpu

   `kvm_vcpu_kick` 的作用是把運行中的 vcpu 退出 guest, 在此處是為了加快該 vcpu 處理中斷請求

3. 處理中斷順便竄改記憶體
   `kvm_check_request` 會查看是否有中斷請求, 順便做中斷植入, 因為我在 `kvm_make_request` 傳的請求是 `KVM_REQ_IRQ_PENDING` 所以, 當我查到此中斷且共用參數符合時, 就會執行記憶體竄改

抽象邏輯

1. 利用 qemu 執行 VM
2. 在 VM 中執行要被篡改 MEM 的 process
3. 從 VM 外調用新 API, 中斷 vcpu 執行, 把數據傳入共用 buffer (ioctl 在 user program 中執行, buffer 在 KVM 實體內存儲)
4. qemu 發現中斷, 額外創建 task 呼叫 ioctl 喚醒 vcpu, 此時在實際啟動前需要處理中斷, 我們在此時進行記憶體竄改
5. 被中斷的 vcpu 使用被篡改的 MEM 被重新啟動

## Part.2 (Userspace Test Programs)

### Testing new ioctl

### Code injection

## Write-up

詳見: [Write-up](./docs/write-up.md)

## 常用指令筆記

生成 write-up.pdf

```
npx md-to-pdf ./docs/write-up.md
```

登入 VM

```
ssh root@localhost -p 2222
```

把檔案移入 VM

```
scp -P 2222 -r ./<target file> root@localhost:/root
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

compile page-types( tool to get guest physical address)

```
# use in vm host
git clone --depth 1 --branch v5.15 https://github.com/torvalds/linux.git
cd linux
# their will be a tool called page-types in `linux/tools/vm`
make -C ./tools/vm/
mv ./tools/vm/page-types ../page-types
```

find all mappings from virtual page number to physical frame number of a process using (gpa)

```
pidof <process name>
./page-types -p <pid> -L
# 根據虛擬地址取得實體地址
./page-types -p <pid> -L | grep <虛擬地址 >> 3 bit>
# 未知地址亂猜（取第一行）
./page-types -p <pid> -L
```
