%string = type [128 x i8]
define i32 @main(i32 noundef %1, [4 x %string] noundef %2) {
%4 = alloca i32, align 4
store i32 %1, ptr %4, align 4
%5 = alloca ptr, align 8
store ptr %2, ptr %5, align 8
%6 = load i32, ptr %4, align 4
ret i32 %6
}
