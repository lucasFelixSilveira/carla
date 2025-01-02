	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"ir.ll"
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.seh_proc main
# %bb.0:                                # %entry
	subq	$56, %rsp
	.seh_stackalloc 56
	.seh_endprologue
	leaq	.L.carla.static.str.0(%rip), %rax
	movq	%rax, 48(%rsp)
	movq	48(%rsp), %rcx
	callq	stdio.println
	movl	$2, 44(%rsp)
	movl	44(%rsp), %eax
	addq	$56, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	stdio.println;
	.scl	2;
	.type	32;
	.endef
	.globl	stdio.println                   # -- Begin function stdio.println
	.p2align	4, 0x90
stdio.println:                          # @stdio.println
.seh_proc stdio.println
# %bb.0:                                # %entry
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%rcx, 64(%rsp)
	movl	$0, 60(%rsp)
	movq	64(%rsp), %rax
	movq	%rax, 48(%rsp)                  # 8-byte Spill
.LBB1_1:                                # %PRINTLNLOOP
                                        # =>This Inner Loop Header: Depth=1
	movq	48(%rsp), %rax                  # 8-byte Reload
	movl	60(%rsp), %ecx
	movl	%ecx, 40(%rsp)                  # 4-byte Spill
	movslq	%ecx, %rcx
	movb	(%rax,%rcx), %al
	movb	%al, 47(%rsp)                   # 1-byte Spill
	cmpb	$0, %al
	je	.LBB1_3
# %bb.2:                                # %PRINTLN.BODY
                                        #   in Loop: Header=BB1_1 Depth=1
	movb	47(%rsp), %al                   # 1-byte Reload
	movsbl	%al, %ecx
	callq	putchar
                                        # kill: def $ecx killed $eax
	movl	40(%rsp), %eax                  # 4-byte Reload
	addl	$1, %eax
	movl	%eax, 60(%rsp)
	jmp	.LBB1_1
.LBB1_3:                                # %PRINTLN.END
	movl	$10, %ecx
	callq	putchar
	nop
	addq	$72, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.section	.rdata,"dr"
.L.carla.static.str.0:                  # @.carla.static.str.0
	.asciz	"Hello, world"

	.addrsig
	.addrsig_sym putchar
	.addrsig_sym stdio.println
	.addrsig_sym .L.carla.static.str.0
