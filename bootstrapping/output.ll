%string = type i8*
declare i32 @puts(%string) nounwind
define i32 @main(i32 noundef %1, %string* noundef %2) {
%4 = alloca i32, align 4
store i32 %1, ptr %4, align 4
%5 = alloca %string*, align 8
store %string* %2, ptr %5, align 8
%6 = alloca i8, align 1
store i8 3, ptr %6, align 1
%7 = load i8, ptr %6, align 1
%8 = sext i8 %7 to i64
%9 = alloca i64, align 8
store i64 1, ptr %9, align 8

%10 = icmp slt i64 %8, 1
br i1 %10, label %EAD0, label %ESB0

ESB0:
%11 = load i64, ptr %9, align 8
%12 = icmp slt i64 %11, %8
br i1 %12, label %L0, label %C0

EAD0:
%13 = load i64, ptr %9, align 8
%14 = icmp sgt i64 %13, %8
br i1 %14, label %L0, label %C0

L0:
%16 = alloca %string, align 8
%17 = load %string*, ptr %5, align 8
%18 = load i64, ptr %9, align 8
%19 = getelementptr inbounds %string, %string* %17, i64 %18
%20 = load %string, ptr %19, align 8
store %string %20, ptr %16, align 8
%21 = load %string, ptr %16, align 8
call i32 @puts(%string %21)

br i1 %10, label %CAD0, label %CSB0

CSB0:
%23 = load i64, ptr %9, align 8
%24 = add nsw i64 %23, 1
store i64 %24, ptr %9, align 8
br label %ESB0

CAD0:
%25 = load i64, ptr %9, align 8
%26 = sub nsw i64 %25, 1
store i64 %26, ptr %9, align 8
br label %EAD0

C0:
ret i32 0
}
