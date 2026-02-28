	.file	"a.c"
	.text
	.globl	main                            # -- Begin function main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	movl	$0, -20(%rbp)
	leaq	-16(%rbp), %rax
	addq	$4, %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movw	$2, (%rax)
	movq	-32(%rbp), %rax
	movswl	(%rax), %eax
	movl	%eax, -36(%rbp)
	movl	-36(%rbp), %eax
	addl	$3, %eax
	movl	%eax, -40(%rbp)
	movl	-40(%rbp), %eax
	subl	$5, %eax
	movl	%eax, -44(%rbp)
	cmpl	$0, -44(%rbp)
	je	.LBB0_2
# %bb.1:
	jmp	.LBB0_3
.LBB0_2:
	jmp	.LBB0_3
.LBB0_3:
	movl	-20(%rbp), %eax
	movl	%eax, -48(%rbp)                 # 4-byte Spill
	movq	%fs:40, %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	.LBB0_5
# %bb.4:                                # %SP_return
	movl	-48(%rbp), %eax                 # 4-byte Reload
	addq	$48, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_5:                                # %CallStackCheckFailBlk
	.cfi_def_cfa %rbp, 16
	callq	__stack_chk_fail@PLT
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 21.1.8"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __stack_chk_fail
