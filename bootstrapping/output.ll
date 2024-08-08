%string = type i8*
define i32 @main(i64 noundef %1, [23 x %string] noundef %2) {
%4 = alloca i64, align 8
store i64 %1, ptr %4, align 8
%5 = alloca ptr, align 8
store ptr %2, ptr %5, align 8
%6 = load i64, ptr %4, align 8
%7 = trunc i64 %6 to i32
ret i32 %7
}
