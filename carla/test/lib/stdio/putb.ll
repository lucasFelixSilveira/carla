@.carla.false_str = private constant [6 x i8] c"false\00", align 1
@.carla.true_str = private constant [5 x i8] c"true\00", align 1
define i32 @stdio.putb(i8 %0) {
entry:
  %2 = icmp eq i8 %0, 0
  br i1 %2, label %IS_FALSE, label %IS_TRUE
  br label %ERROR
IS_FALSE:
  %4 = call i32 @puts(ptr @.carla.false_str)
  ret i32 1
IS_TRUE:
  %5 = call i32 @puts(ptr @.carla.true_str)
  ret i32 1
ERROR:
  ret i32 0
}
