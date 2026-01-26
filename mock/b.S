.text
.globl	main
.type	main, @function
main:
.LFB0:
	pushq	%rbp
	movq	%rsp, %rbp

	movl	$0, %eax
	popq	%rbp
	ret
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 15.2.1 20251112"
	.section	.note.GNU-stack,"",@progbits
