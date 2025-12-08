# com falhas criticas

.include "mock_utils.S"

.text
.globl _start
_start:
    movl $32, %edi

    movq %rsp, %rbp
    sub $2, %rsp

    movb $12, -1(%rbp)
    movb $87, -2(%rbp)
    movq %rsi, %rbp

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

	movq %rdi, %rax
	call print_number

	movb -2(%rsi), %al
	movb %al, -2(%rbp)

	movb -2(%rsi), %al
	movzbq %al, %rax

    mov $2, %rcx
    print_arr_loop:
        movb (%rbp, %rcx), %al
        movzbq %al, %rax
        call print_number
        dec %rcx
        jnz print_arr_loop

.LFE0:
	movq %rbp, %rsp
	popq %rbp
	ret
