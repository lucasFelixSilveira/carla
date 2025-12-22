.text
.type MORGANA.strlen, @function
.globl MORGANA.strlen
MORGANA.strlen:
    movq    %rcx, %r13
    xorq    %rcx, %rcx
    str_len_loop:
        cmpb    $0, (%rsi, %rcx)
        je      str_len_end
        incq    %rcx
        jmp     str_len_loop
    str_len_end:
        movq    %rcx, %rdx
        movq    %r13, %rcx
        xorq    %r13, %r13
        ret
