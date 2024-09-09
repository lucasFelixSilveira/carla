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
%4 = alloca i32, align 4
store i32 0, ptr %4, align 4
%5 = alloca i8, align 1
store i8 2, ptr %5, align 1
%6 = alloca i16, align 2
store i16 4, ptr %6, align 2
br label %E0

E0:
%7 = load i32, ptr %4, align 4
%8 = load i16, ptr %6, align 2
%9 = sext i16 %8 to i32
%11 = icmp slt i32 %7, %9
br i1 %11, label %L0, label %C0

L0:
%12 = load i32, ptr %4, align 4
%13 = load i8, ptr %5, align 1
%14 = sext i8 %13 to i32
%16 = icmp ne i32 %12, %14
br i1 %16, label %L1, label %E1

L1:
%17 = load %string, ptr %2, align 8
call i32 @puts(%string %17)
br label %C2

E1:
%20 = load %string, ptr %3, align 8
call i32 @puts(%string %20)
br label %C2

C2:
%22 = load i32, ptr %4, align 4
%23 = add nsw i32 %22, 1
store i32 %23, ptr %4, align 4
br label %E0

C0:
%25 = load i8, ptr %5, align 1
%26 = sext i8 %25 to i32
ret i32 %26
}
