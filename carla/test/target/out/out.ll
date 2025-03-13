; ModuleID = './target/out/ir.ll'
source_filename = "./target/out/ir.ll"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.29.30158"

@.carla.static.str.0 = private constant [13 x i8] c"Hello, world\00", align 1

; Function Attrs: nofree nounwind
define noundef i32 @main() local_unnamed_addr #0 {
entry:
  br label %.l0.carla.for_body

.l0.carla.for_body:                               ; preds = %entry, %.l0.carla.for_body
  %.03 = phi i32 [ 0, %entry ], [ %1, %.l0.carla.for_body ]
  %0 = tail call i32 @puts(ptr nonnull dereferenceable(1) @.carla.static.str.0)
  %1 = add nuw nsw i32 %.03, 1
  %exitcond.not = icmp eq i32 %1, 10
  br i1 %exitcond.not, label %.l0.carla.for_end, label %.l0.carla.for_body

.l0.carla.for_end:                                ; preds = %.l0.carla.for_body
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
