define i32 @main() {
entry:
%1 = alloca i128, align 16
%2 = add i128 0, 0
%3 = add i128 10, 0
%4 = add i128 3, 0
%5 = add i128 %3, %4
%6 = add i128 6, 0
%7 = add i128 %5, %6
%8 = add i128 2, 0
%9 = mul i128 %7, %8
%10 = add i128 %2, %9
store i128 %10, ptr %1, align 16
ret i32 0
}

