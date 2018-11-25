	.file	"bench.c"
	.text
	.section	.rodata
.LC0:
	.string	"#pages\t proc\t sum"
.LC2:
	.string	"%d\t %.6f\t %ld\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movl	%edi, -52(%rbp)
	movq	%rsi, -64(%rbp)
	leaq	.LC0(%rip), %rdi
	call	puts@PLT
	movl	$1, -48(%rbp)
	jmp	.L2
.L7:
	movl	$1048576, %eax
	cltd
	idivl	-48(%rbp)
	movl	%eax, -36(%rbp)
	call	clock@PLT
	movq	%rax, -24(%rbp)
	movq	$0, -32(%rbp)
	movl	$0, -44(%rbp)
	jmp	.L3
.L6:
	movl	$0, -40(%rbp)
	jmp	.L4
.L5:
	addq	$1, -32(%rbp)
	addl	$1, -40(%rbp)
.L4:
	movl	-40(%rbp), %eax
	cmpl	-48(%rbp), %eax
	jl	.L5
	addl	$1, -44(%rbp)
.L3:
	movl	-44(%rbp), %eax
	cmpl	-36(%rbp), %eax
	jl	.L6
	call	clock@PLT
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	subq	-24(%rbp), %rax
	cvtsi2sdq	%rax, %xmm0
	movsd	.LC1(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -8(%rbp)
	movq	-32(%rbp), %rdx
	movq	-8(%rbp), %rcx
	movl	-48(%rbp), %eax
	movq	%rcx, -72(%rbp)
	movsd	-72(%rbp), %xmm0
	movl	%eax, %esi
	leaq	.LC2(%rip), %rdi
	movl	$1, %eax
	call	printf@PLT
	addl	$1, -48(%rbp)
.L2:
	cmpl	$16, -48(%rbp)
	jle	.L7
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC1:
	.long	0
	.long	1093567616
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
