define i32 @stdio.putc(i8 %0) {
  %2 = sext i8 %0 to i32
  %3 = call i32 @putchar(i32 %2)
  ret i32 %3
}
