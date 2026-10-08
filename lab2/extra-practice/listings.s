# Every Part One listing from lab2-asm.pdf, exactly as printed.
# Labels are renamed per function (.L1 becomes .Ladd3_1) so they do not clash.
.text
.globl add3
add3:
    addq    %rsi, %rdi
    leaq    (%rdi,%rdx), %rax
    ret
.globl affine
affine:
    leal    4(%rdi,%rdi,8), %eax
    ret
.globl times12
times12:
    leal    (%rdi,%rdi,2), %eax
    sall    $2, %eax
    ret
.globl first_plus_third
first_plus_third:
    movq    (%rdi), %rax
    addq    16(%rdi), %rax
    ret
.globl swap
swap:
    movq    (%rdi), %rax
    movq    (%rsi), %rdx
    movq    %rdx, (%rdi)
    movq    %rax, (%rsi)
    ret
.globl byte_sum
byte_sum:
    movzbl  %dil, %edi
    movsbl  %sil, %esi
    leal    (%rdi,%rsi), %eax
    ret
.globl max2
max2:
    cmpl    %edi, %esi
    movl    %edi, %eax
    cmovge  %esi, %eax
    ret
.globl is_below
is_below:
    cmpl    %esi, %edi
    setb    %al
    movzbl  %al, %eax
    ret
.globl abs_diff
abs_diff:
    movl    %edi, %edx
    subl    %esi, %edx
    movl    %esi, %eax
    subl    %edi, %eax
    cmpl    %esi, %edi
    cmovg   %edx, %eax
    ret
.globl power
power:
    testl   %esi, %esi
    jle     .Lpower_1
    movl    $0, %eax
    movl    $1, %edx
.Lpower_2:
    imull   %edi, %edx
    addl    $1, %eax
    cmpl    %eax, %esi
    jne     .Lpower_2
.Lpower_3:
    movl    %edx, %eax
    ret
.Lpower_1:
    movl    $1, %edx
    jmp     .Lpower_3
.globl find
find:
    testl   %esi, %esi
    jle     .Lfind_1
    movslq  %esi, %rsi
    movl    $0, %eax
.Lfind_2:
    cmpl    %edx, (%rdi,%rax,4)
    je      .Lfind_3
    addq    $1, %rax
    cmpq    %rsi, %rax
    jne     .Lfind_2
    movl    $-1, %eax
    ret
.Lfind_1:
    movl    $-1, %eax
.Lfind_3:
    ret
.globl dot
dot:
    testl   %edx, %edx
    jle     .Ldot_1
    movslq  %edx, %rdx
    leaq    0(,%rdx,4), %r8
    movl    $0, %eax
    movl    $0, %ecx
.Ldot_2:
    movl    (%rdi,%rax), %edx
    imull   (%rsi,%rax), %edx
    addl    %edx, %ecx
    addq    $4, %rax
    cmpq    %r8, %rax
    jne     .Ldot_2
.Ldot_3:
    movl    %ecx, %eax
    ret
.Ldot_1:
    movl    $0, %ecx
    jmp     .Ldot_3
.globl div8
div8:
    leal    7(%rdi), %eax
    testl   %edi, %edi
    cmovns  %edi, %eax
    sarl    $3, %eax
    ret
.globl grid_get
grid_get:
    leaq    (%rsi,%rsi,4), %rax
    leaq    (%rdi,%rax,4), %rax
    movl    (%rax,%rdx,4), %eax
    ret
.globl list_sum
list_sum:
    testq   %rdi, %rdi
    je      .Llist_sum_1
    movl    $0, %eax
.Llist_sum_2:
    addq    (%rdi), %rax
    movq    8(%rdi), %rdi
    testq   %rdi, %rdi
    jne     .Llist_sum_2
    ret
.Llist_sum_1:
    movl    $0, %eax
    ret
.globl str_eq
str_eq:
    movzbl  (%rdi), %eax
    testb   %al, %al
    je      .Lstr_eq_1
.Lstr_eq_2:
    cmpb    %al, (%rsi)
    jne     .Lstr_eq_1
    addq    $1, %rdi
    addq    $1, %rsi
    movzbl  (%rdi), %eax
    testb   %al, %al
    jne     .Lstr_eq_2
.Lstr_eq_1:
    cmpb    %al, (%rsi)
    sete    %al
    movzbl  %al, %eax
    ret
.globl apply
apply:
    cmpl    $5, %edi
    ja      .Lapply_1
    movl    %edi, %edi
    leaq    .Lapply_2(%rip), %rdx
    movslq  (%rdx,%rdi,4), %rax
    addq    %rdx, %rax
    jmp     *%rax
.Lapply_2:
    .long   .Lapply_3-.Lapply_2
    .long   .Lapply_4-.Lapply_2
    .long   .Lapply_5-.Lapply_2
    .long   .Lapply_6-.Lapply_2
    .long   .Lapply_7-.Lapply_2
    .long   .Lapply_8-.Lapply_2
.Lapply_3:
    leal    1(%rsi), %eax
    ret
.Lapply_4:
    leal    (%rsi,%rsi,2), %eax
    ret
.Lapply_5:
    leal    -7(%rsi), %eax
    ret
.Lapply_6:
    leal    0(,%rsi,4), %eax
    ret
.Lapply_7:
    movl    %esi, %eax
    xorl    $5, %eax
    ret
.Lapply_8:
    movl    %esi, %eax
    negl    %eax
    ret
.Lapply_1:
    movl    $0, %eax
    ret
.globl fib
fib:
    movl    %edi, %eax
    cmpl    $1, %edi
    jle     .Lfib_1
    pushq   %rbp
    pushq   %rbx
    subq    $8, %rsp
    movl    %edi, %ebx
    leal    -1(%rdi), %edi
    call    fib
    movl    %eax, %ebp
    leal    -2(%rbx), %edi
    call    fib
    addl    %ebp, %eax
    addq    $8, %rsp
    popq    %rbx
    popq    %rbp
    ret
.Lfib_1:
    ret
.section .note.GNU-stack,"",@progbits
