define ptr @heap.alloc(i64 %0) {
  %2 = alloca i64, align 4
  store i64 %0, ptr %2, align 4
  %3 = load i64, ptr %2, align 4
  %5 = call ptr @malloc(i64 %3)
  ret ptr %5
}