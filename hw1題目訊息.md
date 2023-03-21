Looks like you have chosen me as your init?! How honored!
The register of this task is going to be set like this:
x0 == 0x0000000000000000
x1 == 0x0000000001010101
x2 == 0x0000000020202020
x3 == 0x0000000003030303
x4 == 0x0000000040404040
x5 == 0x0000000005050505
x6 == 0x0000000060606060
x7 == 0x0000000007070707
x8 == 0x0000000080808080
x9 == 0x0000000009090909
I am going to execve /sbin/init and let you boot if you can
recognize this register pattern in host KVM and change my
x0 to a nonzero value.
Good luck!
