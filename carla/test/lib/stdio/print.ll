define void @stdio.print(ptr %0) {
  call void @printf(ptr %0)
  ret void
}
