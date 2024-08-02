define i32 @main() {
%1 = alloca i8, align 1
store i8 3, ptr %1, align 1
%2 = load i8, ptr %1, align 1
%3 = sext i8 %2 to i32
%4 = add nsw i32 %3, 2
%5 = trunc i32 %4 to i8
store i8 %5, ptr %1, align 1
%6 = alloca i64, align 8
%7 = load i8, ptr %1, align 1
%8 = sext i8 %7 to i64
store i64 %8, ptr %6, align 8
%9 = load i64, ptr %6, align 8
%10 = trunc i64 %9 to i32
%11 = sub nsw i32 %10, 2
%12 = sext i32 %11 to i64
store i64 %12, ptr %6, align 8
%13 = load i64, ptr %6, align 8
%14 = trunc i64 %13 to i32
%15 = mul nsw i32 %14, 2
%16 = sext i32 %15 to i64
store i64 %16, ptr %6, align 8
%17 = load i64, ptr %6, align 8
%18 = trunc i64 %17 to i32
%19 = sub nsw i32 %18, 5
%20 = sext i32 %19 to i64
store i64 %20, ptr %6, align 8
%21 = load i64, ptr %6, align 8
%22 = trunc i64 %21 to i32
%23 = sub nsw i32 %22, 1
%24 = sext i32 %23 to i64
store i64 %24, ptr %6, align 8
%25 = load i64, ptr %6, align 8
%26 = trunc i64 %25 to i32
ret i32 %26
}
