define i32 @main(i32 %0, ptr %1) {
	%3 = alloca i32, align 4
	%4 = alloca ptr, align 8
	store i32 %3, ptr %0, align 4
	store ptr %4, ptr %1, align 8
	ret i32 0
}

