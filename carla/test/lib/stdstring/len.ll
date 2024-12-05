define i32 @string.len(ptr %0) {
entry:
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  store ptr %0, ptr %2, align 8
  store i32 0, ptr %3, align 4
  %4 = load ptr, ptr %2, align 8
  br label %COUNT

COUNT: ; Início do loop
  %5 = load i32, ptr %3, align 4
  %6 = getelementptr inbounds i8, ptr %4, i32 %5
  %7 = load i8, ptr %6, align 1
  %8 = icmp eq i8 %7, 0
  br i1 %8, label %COUNT.END, label %COUNT.BODY

COUNT.BODY: 
  %9 = add i32 %5, 1 
  store i32 %9, ptr %3, align 4
  br label %COUNT 

COUNT.END: ;
  %10 = load i32, ptr %3, align 4
  ret i32 %10
}
