; ModuleID = './target/out/ir.ll'
source_filename = "./target/out/ir.ll"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.29.30158"

%.carla.struct.0 = type { ptr }

$string.format = comdat any

$_vsprintf_l = comdat any

$_vsnprintf_l = comdat any

$__local_stdio_printf_options = comdat any

@.carla.static.str.0 = private constant [30 x i8] c"{s} said: {s} - Made on Carla\00", align 1
@.carla.static.str.1 = private constant [15 x i8] c"Lucas Silveira\00", align 1
@.carla.static.str.2 = private constant [16 x i8] c"Hello, world <3\00", align 1
@__local_stdio_printf_options._OptionsStorage = internal global i64 0, align 8

define void @person_t.say(%.carla.struct.0 %0, ptr %1) local_unnamed_addr {
  %.fca.0.extract = extractvalue %.carla.struct.0 %0, 0
  %3 = tail call noalias noundef dereferenceable_or_null(128) ptr @malloc(i64 128)
  %4 = tail call i32 (ptr, ptr, ...) @string.format(ptr %3, ptr nonnull @.carla.static.str.0, ptr %.fca.0.extract, ptr %1)
  %5 = tail call i32 @puts(ptr nonnull dereferenceable(1) %3)
  %6 = tail call ptr @free(ptr %3)
  ret void
}

define noundef i32 @main() local_unnamed_addr {
entry:
  %0 = tail call noalias noundef dereferenceable_or_null(128) ptr @malloc(i64 128)
  %1 = tail call i32 (ptr, ptr, ...) @string.format(ptr %0, ptr nonnull @.carla.static.str.0, ptr nonnull @.carla.static.str.1, ptr nonnull @.carla.static.str.2)
  %2 = tail call i32 @puts(ptr nonnull dereferenceable(1) %0)
  %3 = tail call ptr @free(ptr %0)
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #0

; Function Attrs: mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite)
declare noalias noundef ptr @malloc(i64 noundef) local_unnamed_addr #1

; Function Attrs: mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite)
declare void @free(ptr allocptr nocapture noundef) local_unnamed_addr #2

; Function Attrs: mustprogress nofree nounwind willreturn memory(inaccessiblemem: readwrite)
define noalias noundef ptr @heap.alloc(i64 %0) local_unnamed_addr #3 {
  %2 = tail call ptr @malloc(i64 %0)
  ret ptr %2
}

; Function Attrs: nofree norecurse nosync nounwind memory(argmem: readwrite)
define void @carla.replace_placeholders(ptr nocapture writeonly %output, ptr nocapture readonly %input) local_unnamed_addr #4 {
  br label %MAIN_LOOP.outer

MAIN_LOOP.outer:                                  ; preds = %MAIN_LOOP.outer.backedge, %0
  %write_pos.0.ph = phi i32 [ 0, %0 ], [ %next_write_pos.i12, %MAIN_LOOP.outer.backedge ]
  %read_pos.0.ph = phi i32 [ 0, %0 ], [ %next_read_pos_copy, %MAIN_LOOP.outer.backedge ]
  br label %MAIN_LOOP

MAIN_LOOP:                                        ; preds = %MAIN_LOOP.outer, %INVALID_PLACEHOLDER
  %read_pos.0 = phi i32 [ %skip_invalid, %INVALID_PLACEHOLDER ], [ %read_pos.0.ph, %MAIN_LOOP.outer ]
  %1 = sext i32 %read_pos.0 to i64
  %cur_char_ptr = getelementptr inbounds i8, ptr %input, i64 %1
  %cur_char = load i8, ptr %cur_char_ptr, align 1
  switch i8 %cur_char, label %MAIN_LOOP.outer.backedge [
    i8 0, label %END
    i8 123, label %HANDLE_PLACEHOLDER
  ]

HANDLE_PLACEHOLDER:                               ; preds = %MAIN_LOOP
  %next_read_pos = add i32 %read_pos.0, 1
  %2 = sext i32 %next_read_pos to i64
  %next_char_ptr = getelementptr inbounds i8, ptr %input, i64 %2
  %next_char = load i8, ptr %next_char_ptr, align 1
  switch i8 %next_char, label %INVALID_PLACEHOLDER [
    i8 100, label %MAIN_LOOP.outer.backedge.sink.split
    i8 115, label %MAIN_LOOP.outer.backedge.sink.split
  ]

INVALID_PLACEHOLDER:                              ; preds = %HANDLE_PLACEHOLDER
  %skip_invalid = add i32 %read_pos.0, 2
  br label %MAIN_LOOP

MAIN_LOOP.outer.backedge.sink.split:              ; preds = %HANDLE_PLACEHOLDER, %HANDLE_PLACEHOLDER
  %3 = sext i32 %write_pos.0.ph to i64
  %output_ptr.i5 = getelementptr inbounds i8, ptr %output, i64 %3
  store i8 37, ptr %output_ptr.i5, align 1
  %next_write_pos.i6 = add i32 %write_pos.0.ph, 1
  br label %MAIN_LOOP.outer.backedge

MAIN_LOOP.outer.backedge:                         ; preds = %MAIN_LOOP, %MAIN_LOOP.outer.backedge.sink.split
  %write_pos.0.ph.sink = phi i32 [ %next_write_pos.i6, %MAIN_LOOP.outer.backedge.sink.split ], [ %write_pos.0.ph, %MAIN_LOOP ]
  %cur_char.sink = phi i8 [ %next_char, %MAIN_LOOP.outer.backedge.sink.split ], [ %cur_char, %MAIN_LOOP ]
  %.sink45 = phi i32 [ 2, %MAIN_LOOP.outer.backedge.sink.split ], [ 1, %MAIN_LOOP ]
  %.sink = phi i32 [ 3, %MAIN_LOOP.outer.backedge.sink.split ], [ 1, %MAIN_LOOP ]
  %4 = sext i32 %write_pos.0.ph.sink to i64
  %output_ptr.i11 = getelementptr inbounds i8, ptr %output, i64 %4
  store i8 %cur_char.sink, ptr %output_ptr.i11, align 1
  %next_write_pos.i12 = add i32 %write_pos.0.ph, %.sink45
  %next_read_pos_copy = add i32 %read_pos.0, %.sink
  br label %MAIN_LOOP.outer

END:                                              ; preds = %MAIN_LOOP
  %5 = sext i32 %write_pos.0.ph to i64
  %output_ptr.i14 = getelementptr inbounds i8, ptr %output, i64 %5
  store i8 0, ptr %output_ptr.i14, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @write_to_output(ptr nocapture writeonly %output, ptr nocapture %write_pos, i8 %char) local_unnamed_addr #5 {
  %cur_write_pos = load i32, ptr %write_pos, align 4
  %1 = sext i32 %cur_write_pos to i64
  %output_ptr = getelementptr inbounds i8, ptr %output, i64 %1
  store i8 %char, ptr %output_ptr, align 1
  %next_write_pos = add i32 %cur_write_pos, 1
  store i32 %next_write_pos, ptr %write_pos, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @string.format(ptr noundef %0, ptr noundef %1, ...) local_unnamed_addr #6 comdat {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca ptr, align 8
  %7 = call ptr @malloc(i64 1048576)
  call void @carla.replace_placeholders(ptr %7, ptr %1)
  store ptr %7, ptr %3, align 8
  store ptr %0, ptr %4, align 8
  call void @llvm.va_start(ptr %6)
  %8 = load ptr, ptr %6, align 8
  %9 = load ptr, ptr %3, align 8
  %10 = load ptr, ptr %4, align 8
  %11 = call i32 @_vsprintf_l(ptr noundef %10, ptr noundef %9, ptr noundef null, ptr noundef %8)
  store i32 %11, ptr %5, align 4
  call void @llvm.va_end(ptr %6)
  %12 = load i32, ptr %5, align 4
  ret i32 %12
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start(ptr) #7

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsprintf_l(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) local_unnamed_addr #6 comdat {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
  store ptr %2, ptr %6, align 8
  store ptr %1, ptr %7, align 8
  store ptr %0, ptr %8, align 8
  %9 = load ptr, ptr %5, align 8
  %10 = load ptr, ptr %6, align 8
  %11 = load ptr, ptr %7, align 8
  %12 = load ptr, ptr %8, align 8
  %13 = call i32 @_vsnprintf_l(ptr noundef %12, i64 noundef -1, ptr noundef %11, ptr noundef %10, ptr noundef %9)
  ret i32 %13
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end(ptr) #7

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_vsnprintf_l(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3, ptr noundef %4) local_unnamed_addr #6 comdat {
  %6 = alloca ptr, align 8
  %7 = alloca ptr, align 8
  %8 = alloca ptr, align 8
  %9 = alloca i64, align 8
  %10 = alloca ptr, align 8
  %11 = alloca i32, align 4
  store ptr %4, ptr %6, align 8
  store ptr %3, ptr %7, align 8
  store ptr %2, ptr %8, align 8
  store i64 %1, ptr %9, align 8
  store ptr %0, ptr %10, align 8
  %12 = load ptr, ptr %6, align 8
  %13 = load ptr, ptr %7, align 8
  %14 = load ptr, ptr %8, align 8
  %15 = load i64, ptr %9, align 8
  %16 = load ptr, ptr %10, align 8
  %17 = call ptr @__local_stdio_printf_options()
  %18 = load i64, ptr %17, align 8
  %19 = or i64 %18, 1
  %20 = call i32 @__stdio_common_vsprintf(i64 noundef %19, ptr noundef %16, i64 noundef %15, ptr noundef %14, ptr noundef %13, ptr noundef %12)
  store i32 %20, ptr %11, align 4
  %21 = load i32, ptr %11, align 4
  %22 = icmp slt i32 %21, 0
  br i1 %22, label %23, label %24

23:                                               ; preds = %5
  br label %26

24:                                               ; preds = %5
  %25 = load i32, ptr %11, align 4
  br label %26

26:                                               ; preds = %24, %23
  %27 = phi i32 [ -1, %23 ], [ %25, %24 ]
  ret i32 %27
}

declare dso_local i32 @__stdio_common_vsprintf(i64 noundef, ptr noundef, i64 noundef, ptr noundef, ptr noundef, ptr noundef) local_unnamed_addr #8

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local ptr @__local_stdio_printf_options() local_unnamed_addr #6 comdat {
  ret ptr @__local_stdio_printf_options._OptionsStorage
}

; Function Attrs: nofree nounwind
define void @stdio.println(ptr nocapture readonly %0) local_unnamed_addr #0 {
entry:
  %1 = tail call i32 @puts(ptr nonnull dereferenceable(1) %0)
  ret void
}

; Function Attrs: mustprogress willreturn memory(argmem: readwrite, inaccessiblemem: readwrite)
define void @heap.dump(ptr %0) local_unnamed_addr #9 {
  %2 = tail call ptr @free(ptr %0)
  ret void
}

attributes #0 = { nofree nounwind }
attributes #1 = { mustprogress nofree nounwind willreturn allockind("alloc,uninitialized") allocsize(0) memory(inaccessiblemem: readwrite) "alloc-family"="malloc" }
attributes #2 = { mustprogress nounwind willreturn allockind("free") memory(argmem: readwrite, inaccessiblemem: readwrite) "alloc-family"="malloc" }
attributes #3 = { mustprogress nofree nounwind willreturn memory(inaccessiblemem: readwrite) }
attributes #4 = { nofree norecurse nosync nounwind memory(argmem: readwrite) }
attributes #5 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #6 = { noinline nounwind optnone uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { mustprogress nocallback nofree nosync nounwind willreturn }
attributes #8 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { mustprogress willreturn memory(argmem: readwrite, inaccessiblemem: readwrite) }
