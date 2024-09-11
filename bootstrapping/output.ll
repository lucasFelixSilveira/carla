%string = type i8*
declare i32 @puts(%string) nounwind
define i32 @main(i32 noundef %1, %string* noundef %2) {
%4 = alloca i32, align 4
store i32 %1, ptr %4, align 4
%5 = alloca %string*, align 8
store %string* %2, ptr %5, align 8
%6 = alloca i32, align 4
store i32 2, ptr %6, align 4
%7 = load i32, ptr %6, align 4
%8 = sext i32 %7 to i64
%9 = alloca i64, align 8
%10 = alloca i8, align 1
store i64 %8, ptr %9, align 8

%11 = icmp sgt i64 %8, 0
br i1 %11, label %ESB0, label %EAD0

ESB0:
%12 = load i64, ptr %9, align 8
%13 = icmp sgt i64 %12, 0
br i1 %13, label %L0, label %C0

EAD0:
%14 = load i64, ptr %9, align 8
%15 = icmp slt i64 %14, 0
br i1 %15, label %L0, label %C0

L0:
%17 = alloca %string, align 8
%18 = load %string*, ptr %5, align 8
%19 = load i64, ptr %9, align 8
%20 = getelementptr inbounds %string, %string* %18, i64 %19
%21 = load %string, ptr %20, align 8
store %string %21, ptr %17, align 8
%22 = load %string, ptr %17, align 8
call i32 @puts(%string %22)

br i1 %11, label %CSB0, label %CAD0

CSB0:
%24 = load i64, ptr %9, align 8
%25 = sub nsw i64 %24, 1
store i64 %25, ptr %9, align 8
br label %ESB0

CAD0:
%26 = load i64, ptr %9, align 8
%27 = add nsw i64 %26, 1
store i64 %27, ptr %9, align 8
br label %EAD0

C0:
ret i32 0
}
