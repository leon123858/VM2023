git clone https://gitlab.com/qemu-project/qemu.git
cd qemu/
git checkout tags/v7.0.0
./configure --target-list=aarch64-softmmu --disable-werror
make -j8 && make install
cd ../
git clone --depth 1 --branch v5.15 https://github.com/torvalds/linux.git
cd linux
make defconfig && make -j8