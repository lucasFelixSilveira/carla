define i32 @main() {
%0 = alloca i8, align 1
%1 = load i8, ptr %0, align 1
%2 = sext i8 %1 to i32
ret i32 %2
}