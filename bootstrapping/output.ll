define i32 @main() {
%1 = alloca i8, align 1
store i8 0, ptr %1, align 1
store i8 2, ptr %1, align 1
%2 = load i8, ptr %1, align 1
%3 = sext i8 %2 to i32
%4 = mul nsw i32 %3, 4
%5 = trunc i32 %4 to i8
store i8 %5, ptr %1, align 1
%6 = load i8, ptr %1, align 1
%7 = sext i8 %6 to i32
%8 = sub nsw i32 %7, 7
%9 = trunc i32 %8 to i8
store i8 %9, ptr %1, align 1
%10 = alloca i8, align 1
%11 = load i8, ptr %1, align 1
store i8 %11, ptr %10, align 1
%12 = load i8, ptr %10, align 1
%13 = sext i8 %12 to i32
%14 = sub nsw i32 %13, 1
%15 = trunc i32 %14 to i8
store i8 %15, ptr %10, align 1
%16 = load i8, ptr %10, align 1
%17 = sext i8 %16 to i32
ret i32 %17
}
