.text
.globl _start
_start:
	call main
	movq %rax, %rdi
	movq $60, %rax
	syscall

.text
.globl main
.type main, @function
main:
.LFP0:
	pushq %rbp
	movq %rsp, %rbp
	subq $4, %rsp
	movl $21, -4(%rbp)
	movl -4(%rbp), %edi
	addq $2, %rdi
	movq %rdi, %rax
.LFE0:
	movq %rbp, %rsp
	popq %rbp
	ret
