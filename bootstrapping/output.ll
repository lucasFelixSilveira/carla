%string = type i8*
declare i32 @puts(%string) nounwind
define i32 @main(i32 noundef %1, %string* noundef %2) {
%4 = alloca i32, align 4
store i32 %1, ptr %4, align 4
%5 = alloca %string*, align 8
store %string* %2, ptr %5, align 8
%6 = alloca i64, align 8
store i64 2, ptr %6, align 8
br label %E0

E0:
%7 = load i64, ptr %6, align 8
%8 = icmp slt i64 %7, 4
br i1 %8, label %L0, label %C0

L0:
%9 = alloca %string, align 8
%10 = load %string*, ptr %5, align 8
%11 = load i64, ptr %6, align 8
%12 = getelementptr inbounds %string, %string* %10, i64 %11
%13 = load %string, ptr %12, align 8
store %string %13, ptr %9, align 8
%14 = load %string, ptr %9, align 8
call i32 @puts(%string %14)
%16 = load i64, ptr %6, align 8
%17 = add nsw i64 %16, 1
store i64 %17, ptr %6, align 8
br label %E0

C0:
ret i32 0
}
