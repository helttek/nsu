.data

msg:
    .ascii "Hello, world"

.text
    .global _start

_start:
    movq $4, %rax
    movq $1, %rbx
    movq $msg, %rcx
    movq $13, %rdx
    int $0x80

    movq $60, %rax
    xorq %rdi, %rdi
    syscall
