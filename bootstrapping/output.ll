$arr_0 = comdat any
@arr_0 = linkonce_odr dso_local unnamed_addr constant [9 x i8] c"Testando\00", comdat, align 1
%string = type i8*
declare i32 @puts(%string) nounwind
define i32 @main(i32 noundef %1) {
%3 = alloca i32, align 4
store i32 %1, ptr %3, align 4
%4 = alloca %string, align 8
store i8* @arr_0, ptr %4, align 8
%5 = alloca i8, align 1
store i8 0, ptr %5, align 1
br label %E0

E0:
%6 = load i8, ptr %5, align 1
%7 = icmp slt i8 %6, 3
br i1 %7, label %L0, label %C0

L0:
%9 = load %string, ptr %4, align 8
call i32 @puts(%string %9)
%11 = load i8, ptr %5, align 1
%12 = sext i8 %11 to i32
%13 = add nsw i32 %12, 1
%14 = trunc i32 %13 to i8
store i8 %14, ptr %5, align 1
br label %E0

C0:
ret i32 0
}
