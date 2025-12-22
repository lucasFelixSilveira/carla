.globl MORGANA.println
.type MORGANA.println, @function
MORGANA.println:
    call    MORGANA.print
    movq    %rsi, %r14
    xorq    %rsi, %rsi
    movq    $line_feed, %rsi
    call    print
    movq    %r14, %rsi
    xorq    %r14, %r14
    ret
