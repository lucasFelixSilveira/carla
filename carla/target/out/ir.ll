
declare i32 @puts(ptr)

define i32 @main(i32 %0, ptr %1) {
	%3 = alloca i32, align 4
	%4 = alloca ptr, align 8
	store i32 %0, ptr %3, align 4
	store ptr %1, ptr %4, align 8
	%5 = alloca ptr, align 8
	%6 = load ptr, ptr %4, align 8
	%7 = alloca i64, align 8
	store i64 0, ptr %7, align 8
	%8 = load i64, ptr %7, align 8
	%9 = getelementptr inbounds ptr, ptr %6, i64 %8
}
