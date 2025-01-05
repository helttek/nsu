	.file	"main.c"
	.text
	.globl	mythread0
	.type	mythread0, @function
mythread0:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movq	%rdi, -72(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, -52(%rbp)
	leaq	-48(%rbp), %rax
	movl	$812807283, (%rax)
	movb	$0, 4(%rax)
.L2:
	leaq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	puts@PLT
	addl	$1, -52(%rbp)
	movl	-52(%rbp), %eax
	addl	$48, %eax
	movb	%al, -45(%rbp)
	nop
	jmp	.L2
	.cfi_endproc
.LFE6:
	.size	mythread0, .-mythread0
	.section	.rodata
.LC0:
	.string	"%ld\n"
	.text
	.globl	mythread1
	.type	mythread1, @function
mythread1:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movq	%rdi, -72(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	$0, -56(%rbp)
	leaq	-48(%rbp), %rax
	movl	$812807283, (%rax)
	movb	$0, 4(%rax)
.L5:
	movq	-56(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addq	$1, -56(%rbp)
	movq	-56(%rbp), %rax
	addl	$48, %eax
	movb	%al, -45(%rbp)
	nop
	jmp	.L5
	.cfi_endproc
.LFE7:
	.size	mythread1, .-mythread1
	.globl	main
	.type	main, @function
main:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-16(%rbp), %rax
	movl	$0, %ecx
	leaq	mythread1(%rip), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L9
	call	__stack_chk_fail@PLT
.L9:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 13.2.0-23ubuntu4) 13.2.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
