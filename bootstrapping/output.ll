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
%10 = alloca i16, align 2
%11 = load i8, ptr %1, align 1
%12 = sext i8 %11 to i16
store i16 %12, ptr %10, align 2
%13 = load i16, ptr %10, align 2
%14 = sext i16 %13 to i32
%15 = add nsw i32 %14, 3
%16 = trunc i32 %15 to i16
store i16 %16, ptr %10, align 2
%17 = load i16, ptr %10, align 2
%18 = sext i16 %17 to i32
%19 = sub nsw i32 %18, 1
%20 = trunc i32 %19 to i16
store i16 %20, ptr %10, align 2
%21 = load i16, ptr %10, align 2
%22 = sext i16 %21 to i32
ret i32 %22
}
