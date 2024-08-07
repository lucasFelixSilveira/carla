define i32 @main(i32 noundef %1) {
%3 = alloca i32, align 4
store i32 %1, ptr %3, align 4
%4 = load i32, ptr %3, align 4
%5 = mul nsw i32 %4, 2
store i32 %5, ptr %3, align 4
%7 = load i32, ptr %3, align 4
ret i32 %7
}
