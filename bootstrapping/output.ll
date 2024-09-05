%string = type i8*
declare i32 @puts(%string) nounwind
define i32 @main(i32 noundef %1, %string* noundef %2) {
%4 = alloca i32, align 4
store i32 %1, ptr %4, align 4
%5 = alloca %string*, align 8
store %string* %2, ptr %5, align 8
%6 = alloca %string, align 8
%7 = load %string*, ptr %5, align 8
%8 = getelementptr inbounds %string, %string* %7, i64 1
%9 = load %string, ptr %8, align 8
store %string %9, ptr %6, align 8
%10 = load i32, ptr %4, align 4
%11 = icmp sgt i32 %10, 2
br i1 %11, label %L0, label %E0
L0:
%13 = load %string, ptr %6, align 8
call i32 @puts(%string %13)
br label %C1
E0:
%15 = alloca %string, align 8
%16 = load %string*, ptr %5, align 8
%17 = getelementptr inbounds %string, %string* %16, i64 0
%18 = load %string, ptr %17, align 8
store %string %18, ptr %15, align 8
call i32 @puts()
br label %C1
C1:
ret i32 1
}
