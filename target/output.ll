; ModuleID = 'main.crl'
source_filename = "main.crl"

define i8 @main() {
entry:
  %x = alloca i8, align 1
  store i64 117, ptr %x, align 4
}
