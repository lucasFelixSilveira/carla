@.str = private constant [4 x i8] c"%d\0A\00", align 1 
define void @stdio.puti(i32 %n) {
entry:
  %1 = call i32 (i8*, ...) @printf(i8* @.str, i32 %n)
  ret void
}
