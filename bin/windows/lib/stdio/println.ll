define void @stdio.println(ptr %0) {
entry:
  %1 = call i32 @puts(ptr %0)
  ret void
}
