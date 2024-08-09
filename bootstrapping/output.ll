%string = type i8*
declare i32 @puts(%string nocapture) nounwind
define i32 @main(i64 noundef %1, %string* noundef %2) {
%4 = alloca i64, align 8
store i64 %1, ptr %4, align 8
%5 = alloca ptr, align 8
store ptr %2, ptr %5, align 8
%6 = load i64, ptr %4, align 8
%7 = trunc i64 %6 to i32
ret i32 %7
}
