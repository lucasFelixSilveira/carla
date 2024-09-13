	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"output.ll"
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.seh_proc main
# %bb.0:
	pushq	%rbp
	.seh_pushreg %rbp
	subq	$48, %rsp
	.seh_stackalloc 48
	leaq	48(%rsp), %rbp
	.seh_setframe %rbp, 48
	.seh_endprologue
	movl	%ecx, -4(%rbp)
	movq	%rdx, -16(%rbp)
	movb	$12, -17(%rbp)
	movsbq	-17(%rbp), %rax
	movq	%rax, -32(%rbp)
	cmpq	$3, %rax
	setg	%al
	movb	%al, -34(%rbp)                  # 1-byte Spill
	testb	$1, %al
	jne	.LBB0_1
	jmp	.LBB0_2
.LBB0_1:                                # %ESB0
	cmpq	$3, -32(%rbp)
	jg	.LBB0_3
	jmp	.LBB0_6
.LBB0_2:                                # %EAD0
	cmpq	$3, -32(%rbp)
	jge	.LBB0_6
.LBB0_3:                                # %L0
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rax
	movq	-16(%rbp), %rcx
	movq	-32(%rbp), %rdx
	movq	(%rcx,%rdx,8), %rcx
	movq	%rcx, (%rax)
	movq	(%rax), %rcx
	subq	$32, %rsp
	callq	puts
	addq	$32, %rsp
                                        # kill: def $ecx killed $eax
	movb	-34(%rbp), %al                  # 1-byte Reload
	testb	$1, %al
	jne	.LBB0_4
	jmp	.LBB0_5
.LBB0_4:                                # %CSB0
	movq	-32(%rbp), %rax
	subq	$1, %rax
	movq	%rax, -32(%rbp)
	jmp	.LBB0_1
.LBB0_5:                                # %CAD0
	movq	-32(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -32(%rbp)
	jmp	.LBB0_2
.LBB0_6:                                # %C0
	xorl	%eax, %eax
	movq	%rbp, %rsp
	popq	%rbp
	retq
	.seh_endproc
                                        # -- End function
	.addrsig
	.addrsig_sym puts
