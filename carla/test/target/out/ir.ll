define i32 @main(i32 %0, ptr %1) {
	%3 = alloca i32, align 4
	%4 = alloca ptr, align 8
	store i32 %0, ptr %3, align 4
	store ptr %1, ptr %4, align 8
}
