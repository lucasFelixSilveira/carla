define i32 @main() {
%1 = alloca i16, align 2
store i16 4, ptr %1, align 2
store i16 2, ptr %1, align 2
%2 = load i16, ptr %1, align 2
%3 = sext i16 %2 to i32
%4 = add nsw i32 %3, 5
%5 = trunc i32 %4 to i16
store i16 %5, ptr %1, align 2
%6 = load i16, ptr %1, align 2
%7 = sext i16 %6 to i32
ret i32 %7
}
