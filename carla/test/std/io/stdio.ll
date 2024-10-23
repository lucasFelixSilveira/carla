@linefeed = private unnamed_addr constant [2 x i8] c"\0a\00", align 1

declare i32 @puts(ptr)
define void @println(ptr %0) {
  %2 = call i32 @puts(%str)
  %3 = call i32 @puts(@linefeed)
  ret void
}