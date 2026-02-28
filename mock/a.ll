; ModuleID = 'main'
declare i32 @puts(i8*)

@.alert_str = private constant [6 x i8] c"Alert\00"

define i32 @main() {
entry:
  %x = alloca { i32, i16 }
  %addr = getelementptr inbounds { i32, i16 }, { i32, i16 }* %x, i32 0, i32 1
  store i16 2, i16* %addr
  %value = load i16, i16* %addr
  %z = add i16 %value, 3
  %y = sub i16 %z, 5
  %cond = icmp ne i16 %y, 0
  br i1 %cond, label %zero, label %alert_block

alert_block:
  %strptr = getelementptr [6 x i8], [6 x i8]* @.alert_str, i32 0, i32 0
  call i32 @puts(i8* %strptr)
  br label %zero

zero:
  ret i32 0
}
