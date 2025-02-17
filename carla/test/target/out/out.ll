; ModuleID = './target/out/ir.ll'
source_filename = "./target/out/ir.ll"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.29.30158"

@.carla.static.str.0 = private constant [6 x i8] c"Lucas\00", align 1

; Function Attrs: nofree nounwind
define noundef i32 @main() local_unnamed_addr #0 {
entry:
  %0 = tail call i32 @puts(ptr nonnull dereferenceable(1) @.carla.static.str.0)
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #0

; Function Attrs: nofree nounwind
define void @stdio.println(ptr nocapture readonly %0) local_unnamed_addr #0 {
entry:
  %1 = tail call i32 @puts(ptr nonnull dereferenceable(1) %0)
  ret void
}

attributes #0 = { nofree nounwind }
