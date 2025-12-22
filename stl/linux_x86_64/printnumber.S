.globl MORGANA.printnumber
.type MORGANA.printnumber, @function
MORGANA.printnumber:
    pushq   %rbp
    movq    %rax, %r9
    movq    %rsp, %rbp
    movq    %rdi, %r13
    movq    %rdi, %rax
    movq    %rbp, %rcx
    subq    $20, %rsp
    subq    $8, %rcx

    print_loop:
        xorl    %edx, %edx
        movl    $10, %ebx
        divl    %ebx # rax/ebx
        movq    %rdx, %rdi
        addb    $48 ,%dil
        movb    %dil, (%rcx)
        decq    %rcx

        cmp     $0, %rax
        jne     print_loop
        je      print_final

    print_final:
        incq    %rcx
        leaq    (%rcx), %rsi
        call    MORGANA.println
        addq    $20, %rsp
        popq    %rbp
        movq    %r13, %rdi
        ret
