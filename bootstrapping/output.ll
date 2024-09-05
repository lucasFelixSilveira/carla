%string = type i8*
declare i32 @puts(%string) nounwind
define i32 @main(i32 noundef %1, %string* noundef %2) {
%4 = alloca i32, align 4
store i32 %1, ptr %4, align 4
%5 = alloca %string*, align 8
store %string* %2, ptr %5, align 8
%6 = load i32, ptr %4, align 4
%7 = icmp sgt i32 %6, 2
br i1 %7, label %L0, label %E0
L0:
%9 = alloca %string, align 8
%10 = load %string*, ptr %5, align 8
%11 = getelementptr inbounds %string, %string* %10, i64 1
%12 = load %string, ptr %11, align 8
store %string %12, ptr %9, align 8
%13 = load %string, ptr %9, align 8
call i32 @puts(%string %13)
br label %C1
E0:
%15 = alloca %string, align 8
%16 = load %string*, ptr %5, align 8
%17 = getelementptr inbounds %string, %string* %16, i64 0
%18 = load %string, ptr %17, align 8
store %string %18, ptr %15, align 8
%19 = load %string, ptr %15, align 8
call i32 @puts(%string %19)
br label %C1
C1:
ret i32 1
}
