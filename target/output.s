.text
.globl _start
_start:
	call main
	mov %rax, %rdi
	mov $60, %rax
	syscall

.text
.global main
.type main, @function
main:
.LFP0:
	push %rbp
	mov %rsp, %rbp
	sub $16, %rsp

.LFE0:
	mov %rbp, %rsp
	pop %rbp
	ret
