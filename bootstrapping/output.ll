$arr_0 = comdat any
@arr_0 = linkonce_odr dso_local unnamed_addr constant [11 x i8] c"teste\0asexo\00", comdat, align 1
%string = type i8*
declare i32 @puts(%string) nounwind
define i32 @main() {
%2 = alloca %string, align 8
store i8* @arr_0, ptr %2, align 8
%3 = alloca i8, align 1
store i8 0, ptr %3, align 1
br label %E0

E0:
%4 = load i8, ptr %3, align 1
%5 = icmp slt i8 %4, 5
br i1 %5, label %L0, label %C0

L0:
%7 = load %string, ptr %2, align 8
call i32 @puts(%string %7)
%9 = load i8, ptr %3, align 1
%10 = sext i8 %9 to i32
%11 = add nsw i32 %10, 1
%12 = trunc i32 %11 to i8
store i8 %12, ptr %3, align 1
br label %E0

C0:
ret i32 0
}
