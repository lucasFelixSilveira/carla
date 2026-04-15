.globl _start
_start:
	movq %rsp, %rbp
	call main
	leave
	
	movq %rax, %rdi
	movq $60, %rax
	syscall
	
.globl main
main:
.LFP0:
	pushq %rbp
	movq %rsp, %rbp
	movq $14, %rdi
	jmp .LFE0
.LFE0:
	.size main, .LFE0 - main
	movq %rdi, %rax
	leave
	ret
