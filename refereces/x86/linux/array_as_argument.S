.text
.globl _start
_start:
    movq %rsp, %rbp
    sub $2, %rsp
    movb $11, -1(%rbp)
    movb $24, -2(%rbp)

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
	subq $2, %rsp

	movb -2(%rdi), %al
	movb %al, -2(%rbp)

	movb -2(%rdi), %al
	movzbq %al, %rax
.LFE0:
	movq %rbp, %rsp
	popq %rbp
	ret
