define void @stdio.print(ptr %0) {
entry:
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  store ptr %0, ptr %2, align 8
  store i32 0, ptr %3, align 4
  %4 = load ptr, ptr %2, align 8
  br label %PRINT

PRINT: ; Início do loop
  %5 = load i32, ptr %3, align 4
  %6 = getelementptr inbounds i8, ptr %4, i32 %5
  %7 = load i8, ptr %6, align 1
  %8 = icmp eq i8 %7, 0
  br i1 %8, label %PRINTLN.END, label %PRINTLN.BODY

PRINTLN.BODY: ; Corpo do loop
  %9 = sext i8 %7 to i32
  %10 = call i32 @putchar(i32 %9)
  %11 = add i32 %5, 1 ; Incrementa o índice
  store i32 %11, ptr %3, align 4
  br label %PRINT ; Volta para o início do loop

PRINTLN.END: ; Fim do loop
  ret void
}
