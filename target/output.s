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
	movq $1, %rax
	movq $1, %rdi
	movq $.fn0._0, %rsi
	movq $14, %rdx
	syscall
.LFE0:
	.size main, .LFE0 - main
	movq %rdi, %rax
	leave
	ret
.data
	.fn0._0: .string "Hello, world"
