%string = type i8*
declare i32 @puts(%string) nounwind
define i32 @main(i32 noundef %1, %string* noundef %2) {
%4 = alloca i32, align 4
store i32 %1, ptr %4, align 4
%5 = alloca %string*, align 8
store %string* %2, ptr %5, align 8
%6 = alloca i8, align 1
store i8 2, ptr %6, align 1
%7 = load i8, ptr %6, align 1
%8 = sext i8 %7 to i64
%9 = alloca i64, align 8
store i64 0, ptr %9, align 8
br label %E0

E0:
%10 = load i8, ptr %6, align 1
%11 = sext i8 %10 to i64
%13 = load i64, ptr %9, align 8
%14 = icmp sgt i64 %11, %13
br i1 %14, label %L0, label %C0

L0:
%15 = alloca %string, align 8
%16 = load %string*, ptr %5, align 8
%17 = load i64, ptr %9, align 8
%18 = getelementptr inbounds %string, %string* %16, i64 %17
%19 = load %string, ptr %18, align 8
store %string %19, ptr %15, align 8
%20 = load %string, ptr %15, align 8
call i32 @puts(%string %20)
%22 = load i64, ptr %9, align 8
%23 = add nsw i64 %22, 1
store i64 %23, ptr %9, align 8
br label %E0

C0:
ret i32 0
}
