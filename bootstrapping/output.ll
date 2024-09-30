define i32 @main(i32 %0, i8 %1) {
	%3 = alloca i32, align 4
	%4 = alloca i8, align 1
	store i32 %3, ptr %0, align 4
	store i8 %4, ptr %1, align 1
	ret i32 0
}

