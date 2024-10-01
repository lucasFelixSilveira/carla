define i32 @main(i32 %0, ptr %1) {
	%3 = alloca i32, align 4
	%4 = alloca ptr, align 8
	store i32 %0, ptr %3, align 4
	store ptr %1, ptr %4, align 8
	%5 = alloca i128, align 16
	%6 = load i32, ptr %3, align 4
	%7 = sext i32 %6 to i128
	store i128 %7, ptr %5, align 16
	%8 = load i128, ptr %5, align 16
	%9 = trunc i128 %8 to i32
	ret i32 %9
}

