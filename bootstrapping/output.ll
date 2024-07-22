define i32 @main() {
%1 = alloca i16, align 2
store i16 4, ptr %1, align 2
store i16 2, ptr %1, align 2
%2 = load i16, ptr %1, align 2
%3 = sext i16 %2 to i32
ret i32 %3
}
