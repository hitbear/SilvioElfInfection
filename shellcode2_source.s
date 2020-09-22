# this is at%t syntax
# compile with 
# as shellcode2_source.s -o shellcode2.o
# link with 
# ld -o shellcode2.out shellcode2.o


.globl _start
    _start:

        mov %rsp, %rbp # current stack pointer address
        sub $0x10, %rsp # 'reserve' 16 bytes by subtracting 0x10 from the current stack pointer address
        

        # fill the 'reserved' location
        movb $0x74, -0xf(%rbp)
        movb $0x68, -0xe(%rbp)
        movb $0x69, -0xd(%rbp)
        movb $0x73, -0xc(%rbp)
        movb $0x20, -0xb(%rbp)
        movb $0x69, -0xa(%rbp)
        movb $0x73, -0x9(%rbp)
        movb $0x20, -0x8(%rbp)
        movb $0x61, -0x7(%rbp)
        movb $0x20, -0x6(%rbp)
        movb $0x73, -0x5(%rbp)
        movb $0x74, -0x4(%rbp)
        movb $0x72, -0x3(%rbp)
        movb $0x69, -0x2(%rbp)
        movb $0x6e, -0x1(%rbp)
        movb $0x67, -0x0(%rbp)


        lea -0xf(%rbp), %rsi # LEA means load effective address
        mov $0x10, %edx
        mov $0x1, %edi
        mov $0x1, %rax
        syscall
    

        movl $60,%eax		# exit
        syscall


