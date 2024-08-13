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
	subq	$56, %rsp
	.seh_stackalloc 56
	.seh_endprologue
	movl	%ecx, 52(%rsp)
	movq	%rdx, 40(%rsp)
	movq	40(%rsp), %rax
	movq	8(%rax), %rax
	movq	%rax, 32(%rsp)
	movq	32(%rsp), %rcx
	callq	puts
	movl	$1, %eax
	addq	$56, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.addrsig
	.addrsig_sym puts
