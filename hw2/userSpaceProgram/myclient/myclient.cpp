#include <cstdint>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

struct kvm_arm_write_gpa_args {
  uint32_t vmid; // the vmid that you, as the host, want to write to (default 1)
  uint64_t gpa;  // the gpa(guest physical address) of the guest
  uint64_t data; // address of the payload in host user space
  uint64_t size; // size of the payload
};
#define KVMIO 0xAE
#define KVM_ARM_WRITE_GPA _IOW(KVMIO, 0xfe, struct kvm_arm_write_gpa_args)

int main() {
  const char *data = "I am cool baby.";
  struct kvm_arm_write_gpa_args tmp = {1, 0, (uint64_t)data,
                                       strlen(data) * sizeof(char)};
  int fd;
  int32_t value, number;
  printf("*********************************\n");

  printf("\nOpening Driver\n");
  fd = open("/dev/kvm", O_RDWR);
  if (fd < 0) {
    printf("Cannot open device file...\n");
    return 0;
  }
  int result = ioctl(fd, KVM_ARM_WRITE_GPA, &tmp);
  if (result == -1) {
    printf("errON[%d]errMsg[%s]\n", errno, strerror(errno));
  }
  printf("Value is %d\n", result);
  printf("Closing Driver\n");
  close(fd);
}