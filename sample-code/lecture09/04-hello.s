# Hello, world with no C library: two system calls and nothing else.
#
# Linux x86-64 only. The system call numbers (1 = write, 60 = exit) belong to
# Linux; macOS numbers its system calls differently.
#
#   make linux        (or: gcc -nostdlib -static 04-hello.s -o 04-hello)
#   ./04-hello
#   strace ./04-hello

        .section .rodata
msg:    .ascii  "hello, world\n"
        len = . - msg                 # 13 bytes

        .text
        .globl  _start
_start:                               # the kernel starts us here, not at main
        movq    $1, %rax              # system call 1 is write
        movq    $1, %rdi              # argument 1: file descriptor 1 (stdout)
        leaq    msg(%rip), %rsi       # argument 2: address of the bytes
        movq    $len, %rdx            # argument 3: how many bytes
        syscall                       # enter the kernel; result comes back in %rax

        movq    $60, %rax             # system call 60 is exit
        movq    $0, %rdi              # argument 1: exit status 0
        syscall                       # does not return
