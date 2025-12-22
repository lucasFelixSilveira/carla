.text
.type MORGANA.print, @function
.globl MORGANA.print
MORGANA.print:
    movq    %rdi, %r8
    movq    $1, %rax
    movq    $1, %rdi
    call    MORGANA.strlen
    syscall
    movq    %r8, %rdi
    ret
