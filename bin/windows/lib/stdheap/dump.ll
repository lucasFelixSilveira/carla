define void @heap.dump(ptr %0) {
  call ptr @free(ptr %0)
  ret void
}