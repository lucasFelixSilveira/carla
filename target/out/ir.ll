; This is a code generated by the Carla programming language

define i32 @main() {
entry:
; [CARLA DEBUG]: Function added to stack: println.
  %1 = alloca ptr, align 8
  %2 = getelementptr inbounds [13 x i8], ptr @.carla.static.str.0, i32 0, i32 0
  store ptr %2, ptr %1, align 8
  %3 = load ptr, ptr %1, align 8
; [CARLA DEBUG]: The argument is a text
  %4 = call i32 @stdio.println(ptr %3)
; [CARLA DEBUG]: 0 items remaining in the stack.
; [CARLA DEBUG]: Default success return, in case the return is not done manually.
  ret i32 0
}
; [CARLA DEBUG]: Tab n: 1
@.carla.static.str.0 = private constant [13 x i8] c"hello, world\00", align 1

; Extern functions
declare i32 @puts(ptr)
declare i32 @printf(ptr)
declare i32 @putchar(i32)
declare i32 @sprintf(ptr, ptr, ...)
; stdio Initialized
define void @stdio.println(ptr %0) {
entry:
  %1 = call i32 @puts(ptr %0)
  ret void
}

