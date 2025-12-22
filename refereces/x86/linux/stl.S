.data
    line_feed: .asciz "\n"

.text
str_len:
    mov     %rcx, %r13
    xor     %rcx, %rcx
    str_len_loop:
        cmpb    $0, (%rsi, %rcx)
        je      str_len_end
        inc     %rcx
        jmp     str_len_loop
    str_len_end:
        mov     %rcx, %rdx
        mov     %r13, %rcx
        xor     %r13, %r13
        ret

print:
    mov     %rdi, %r8
    mov     $1, %rax
    mov     $1, %rdi
    call    str_len
    syscall
    mov     %r8, %rdi
    ret

.globl print_ln
.type print_ln, @function
print_ln:

    call    print
    mov     %rsi, %r14
    xor     %rsi, %rsi
    mov     $line_feed, %rsi
    call    print
    mov     %r14, %rsi
    xor     %r14, %r14
    ret

print_number:# (2 ^ 32) - 1
#               4294967295

    push    %rbp
    mov     %rax, %r9
    mov     %rsp, %rbp
    mov     %rdi, %r13
    mov     %rdi, %rax
    mov     %rbp, %rcx
    sub     $20, %rsp
    sub     $8, %rcx
    # sub $20,%rsp # 8 bit inicio 12 data
    print_loop:
        xor     %edx, %edx
        mov     $10, %ebx
        div     %ebx # rax/ebx
        mov     %rdx, %rdi
        addb    $48 ,%dil
        movb    %dil, (%rcx)
        dec     %rcx

        cmp     $0, %rax
        jne     print_loop
        je      print_final

    print_final:
        inc     %rcx
        lea     (%rcx), %rsi
        call    print_ln
        add     $20, %rsp
        pop     %rbp
        mov     %r13, %rdi
        ret
