$arr_0 = comdat any
@arr_0 = linkonce_odr dso_local unnamed_addr constant [14 x i8] c"Hello, world!\00", comdat, align 1
%string = type i8*
declare i32 @puts(%string) nounwind
define i32 @main() {
%2 = alloca %string, align 8
store i8* @arr_0, ptr %2, align 8
%3 = alloca i64, align 8
store i64 0, ptr %3, align 8
br label %E0

E0:
%4 = load i64, ptr %3, align 8
%5 = icmp slt i64 %4, 2
br i1 %5, label %L0, label %C0

L0:
%6 = load %string, ptr %2, align 8
call i32 @puts(%string %6)
%8 = load i64, ptr %3, align 8
%9 = add nsw i64 %8, 1
store i64 %9, ptr %3, align 8
br label %E0

C0:
ret i32 0
}
