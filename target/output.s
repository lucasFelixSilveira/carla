.text
.globl _start
_start:
	call main
	movq %rax, %rdi
	movq $60, %rax
	syscall
	
.text
.globl main
main:
.LFP0:
	pushq %rbp
	movq %rsp, %rbp
	movq $.fn0._1, -8(%rbp)
	movq -8(%rbp), %rdi
	call println
.LFE0:
	.size main, .LFE0 - main
	movq %rdi, %rax
	leave
	ret
.data
	.fn0._1: .string "Hello, world! My name is Lucas"
