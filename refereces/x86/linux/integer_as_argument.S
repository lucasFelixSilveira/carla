.include "mock_utils.S"

.text
.globl _start
_start:
    movl $126, %edi

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

	movl %edi, -4(%rbp)

	lea -4(%rbp), %rax
	movzbq %al, %rax
	call print_number

	mov $0, %rax
.LFE0:
	movq %rbp, %rsp
	popq %rbp
	ret
