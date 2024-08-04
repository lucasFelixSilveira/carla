define i32 @main(i32 noundef %1) {
%3 = alloca i32, align 4
store i32 %1, ptr %3, align 4
%4 = load i32, ptr %3, align 4
ret i32 %4
}
