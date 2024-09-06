$arr_1 = comdat any
@arr_1 = linkonce_odr dso_local unnamed_addr constant [7 x i8] c"Second\00", comdat, align 1
$arr_0 = comdat any
@arr_0 = linkonce_odr dso_local unnamed_addr constant [9 x i8] c"Testando\00", comdat, align 1
%string = type i8*
declare i32 @puts(%string) nounwind
define i32 @main() {
%2 = alloca %string, align 8
store i8* @arr_0, ptr %2, align 8
%3 = alloca %string, align 8
store i8* @arr_1, ptr %3, align 8
%4 = alloca i8, align 1
store i8 0, ptr %4, align 1
br label %E0

E0:
%5 = load i8, ptr %4, align 1
%6 = icmp slt i8 %5, 4
br i1 %6, label %L0, label %C0

L0:
%8 = load i8, ptr %4, align 1
%9 = icmp ne i8 %8, 2
br i1 %9, label %L1, label %E1

L1:
%11 = load %string, ptr %2, align 8
call i32 @puts(%string %11)
br label %C2

E1:
%14 = load %string, ptr %3, align 8
call i32 @puts(%string %14)
br label %C2

C2:
%16 = load i8, ptr %4, align 1
%17 = sext i8 %16 to i32
%18 = add nsw i32 %17, 1
%19 = trunc i32 %18 to i8
store i8 %19, ptr %4, align 1
br label %E0

C0:
ret i32 0
}
