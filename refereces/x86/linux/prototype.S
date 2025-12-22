.data
    msg: .string "Test if the print is ready"
.text
.globl _start
_start:
    call main
    mov %rax, %rdi
    mov $60, %rax
    syscall

.text
.type main, @function
.globl main
main:
.LFP0:
    push %rbp
    mov %rsp, %rbp
    sub $16, %rsp

    mov $msg, %rsi
    call print_ln

    mov $0, %rax
.LFE0:
	movq %rbp, %rsp
	popq %rbp
	ret
