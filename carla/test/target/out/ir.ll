; This is a code generated by the Carla programming language

define i32 @main() {
entry:
  %1 = alloca ptr, align 8
; [CARLA DEBUG]: Function added to stack: alloc.
  %2 = alloca i64, align 8
  store i64 128, ptr %2, align 8
  %3 = load i64, ptr %2, align 8
; [CARLA DEBUG]: The argument is a number
  %4 = call ptr @heap.alloc(i64 %3)
; [CARLA DEBUG]: 0 items remaining in the stack.
  store ptr %4, ptr %1, align 8
; [CARLA DEBUG]: Function added to stack: format.
  %5 = load ptr, ptr %1, align 8
  %6 = alloca ptr, align 8
  %7 = getelementptr inbounds [43 x i8], ptr @.carla.static.str.0, i32 0, i32 0
  store ptr %7, ptr %6, align 8
  %8 = load ptr, ptr %6, align 8
; [CARLA DEBUG]: The argument is a text
  %9 = alloca ptr, align 8
  %10 = getelementptr inbounds [6 x i8], ptr @.carla.static.str.1, i32 0, i32 0
  store ptr %10, ptr %9, align 8
  %11 = load ptr, ptr %9, align 8
; [CARLA DEBUG]: The argument is a text
  %12 = alloca i64, align 8
  store i64 15, ptr %12, align 8
  %13 = load i64, ptr %12, align 8
; [CARLA DEBUG]: Pre-compiled obvious operation
  %14 = call i32 @string.format(ptr %5, ptr %8, ptr %11, i64 %13)
; [CARLA DEBUG]: 0 items remaining in the stack.
; [CARLA DEBUG]: Function added to stack: println.
  %15 = load ptr, ptr %1, align 8
  %16 = call i32 @stdio.println(ptr %15)
; [CARLA DEBUG]: 0 items remaining in the stack.
; [CARLA DEBUG]: Function added to stack: dump.
  %17 = load ptr, ptr %1, align 8
  call void @heap.dump(ptr %17)
; [CARLA DEBUG]: 0 items remaining in the stack.
  %18 = alloca i32, align 4
  store i32 0, ptr %18, align 4
  %19 = load i32, ptr %18, align 4
  ret i32 %19
}

@.carla.static.str.0 = private constant [43 x i8] c"Hello, world! My name is {s}, and i am {d}\00", align 1
@.carla.static.str.1 = private constant [6 x i8] c"Lucas\00", align 1

; Extern functions
declare i32 @puts(ptr)
declare i32 @putchar(i32)
declare i32 @sprintf(ptr, ptr, ...)
; stdio Initialized
declare ptr @malloc(i64)
declare void @free(ptr)
; stdheap Initialized
declare i32 @strlen(i8* %str) 
; stdstring Initialized
define ptr @heap.alloc(i64 %0) {
  %2 = alloca i64, align 4
  store i64 %0, ptr %2, align 4
  %3 = load i64, ptr %2, align 4
  %5 = call ptr @malloc(i64 %3)
  ret ptr %5
}
define void @carla.replace_placeholders(ptr %output, ptr %input) {
  ; Aloca variáveis para controle
  %read_pos = alloca i32, align 4       ; Posição de leitura no input
  %write_pos = alloca i32, align 4      ; Posição de escrita no output
  store i32 0, ptr %read_pos, align 4   ; Inicializa posição de leitura
  store i32 0, ptr %write_pos, align 4  ; Inicializa posição de escrita

  ; Inicia o loop principal
  br label %MAIN_LOOP

MAIN_LOOP:
  ; Lê o caractere atual do input
  %cur_read_pos = load i32, ptr %read_pos, align 4
  %cur_char_ptr = getelementptr inbounds i8, ptr %input, i32 %cur_read_pos
  %cur_char = load i8, ptr %cur_char_ptr, align 1

  ; Verifica se chegou ao final da string
  %is_end = icmp eq i8 %cur_char, 0
  br i1 %is_end, label %END, label %PROCESS_CHAR

PROCESS_CHAR:
  ; Verifica se é '{', indicando início de um placeholder
  %is_placeholder_start = icmp eq i8 %cur_char, 123 ; '{'
  br i1 %is_placeholder_start, label %HANDLE_PLACEHOLDER, label %COPY_CHAR

HANDLE_PLACEHOLDER:
  ; Avança para o próximo caractere dentro do placeholder
  %next_read_pos = add i32 %cur_read_pos, 1
  store i32 %next_read_pos, ptr %read_pos, align 4

  ; Lê o próximo caractere para identificar o tipo de placeholder
  %next_char_ptr = getelementptr inbounds i8, ptr %input, i32 %next_read_pos
  %next_char = load i8, ptr %next_char_ptr, align 1

  ; Verifica o tipo de placeholder
  %is_s = icmp eq i8 %next_char, 115 ; 's'
  %is_d = icmp eq i8 %next_char, 100 ; 'd'

  br i1 %is_d, label %REPLACE_D, label %CHECK_S

CHECK_S:
  br i1 %is_s, label %REPLACE_S, label %INVALID_PLACEHOLDER

REPLACE_D:
  ; Escreve '%d' no output
  call void @write_to_output(ptr %output, ptr %write_pos, i8 37) ; '%'
  call void @write_to_output(ptr %output, ptr %write_pos, i8 100) ; 'd'

  ; Salta o '}' e volta ao loop principal
  %skip_to_end = add i32 %next_read_pos, 2 ; Pula 'd' e '}'
  store i32 %skip_to_end, ptr %read_pos, align 4
  br label %MAIN_LOOP

REPLACE_S:
  ; Escreve '%s' no output
  call void @write_to_output(ptr %output, ptr %write_pos, i8 37) ; '%'
  call void @write_to_output(ptr %output, ptr %write_pos, i8 115) ; 's'

  ; Salta o '}' e volta ao loop principal
  %skip_to_end_s = add i32 %next_read_pos, 2 ; Pula 's' e '}'
  store i32 %skip_to_end_s, ptr %read_pos, align 4
  br label %MAIN_LOOP

INVALID_PLACEHOLDER:
  ; Placeholder inválido: apenas avança
  %skip_invalid = add i32 %next_read_pos, 1
  store i32 %skip_invalid, ptr %read_pos, align 4
  br label %MAIN_LOOP

COPY_CHAR:
  ; Copia o caractere atual para o output
  call void @write_to_output(ptr %output, ptr %write_pos, i8 %cur_char)

  ; Avança a posição de leitura
  %next_read_pos_copy = add i32 %cur_read_pos, 1
  store i32 %next_read_pos_copy, ptr %read_pos, align 4
  br label %MAIN_LOOP

END:
  ; Finaliza a string no output com null terminator
  call void @write_to_output(ptr %output, ptr %write_pos, i8 0)
  ret void
}

; Função auxiliar para escrever um caractere no output e avançar a posição de escrita
define void @write_to_output(ptr %output, ptr %write_pos, i8 %char) {
  %cur_write_pos = load i32, ptr %write_pos, align 4
  %output_ptr = getelementptr inbounds i8, ptr %output, i32 %cur_write_pos
  store i8 %char, ptr %output_ptr, align 1
  %next_write_pos = add i32 %cur_write_pos, 1
  store i32 %next_write_pos, ptr %write_pos, align 4
  ret void
}


;; Codigo incorporado do CLANG

; ModuleID = 'a.c'
$string.format = comdat any
$vsprintf = comdat any
$_snprintf = comdat any
$_vsnprintf = comdat any
$_vsprintf_l = comdat any
$_vsnprintf_l = comdat any
$__local_stdio_printf_options = comdat any
@__local_stdio_printf_options._OptionsStorage = internal global i64 0, align 8

define linkonce_odr dso_local i32 @string.format(ptr noundef %0, ptr noundef %1, ...) #0 comdat {
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

define linkonce_odr dso_local i32 @vsprintf(ptr noundef %0, ptr noundef %1, ptr noundef %2) #0 comdat {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  store ptr %2, ptr %4, align 8
  store ptr %1, ptr %5, align 8
  store ptr %0, ptr %6, align 8
  %7 = load ptr, ptr %4, align 8
  %8 = load ptr, ptr %5, align 8
  %9 = load ptr, ptr %6, align 8
  %10 = call i32 @_vsnprintf_l(ptr noundef %9, i64 noundef -1, ptr noundef %8, ptr noundef null, ptr noundef %7)
  ret i32 %10
}

define linkonce_odr dso_local i32 @_snprintf(ptr noundef %0, i64 noundef %1, ptr noundef %2, ...) #0 comdat {
  %4 = alloca ptr, align 8
  %5 = alloca i64, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca ptr, align 8
  store ptr %2, ptr %4, align 8
  store i64 %1, ptr %5, align 8
  store ptr %0, ptr %6, align 8
  call void @llvm.va_start(ptr %8)
  %9 = load ptr, ptr %8, align 8
  %10 = load ptr, ptr %4, align 8
  %11 = load i64, ptr %5, align 8
  %12 = load ptr, ptr %6, align 8
  %13 = call i32 @_vsnprintf(ptr noundef %12, i64 noundef %11, ptr noundef %10, ptr noundef %9)
  store i32 %13, ptr %7, align 4
  call void @llvm.va_end(ptr %8)
  %14 = load i32, ptr %7, align 4
  ret i32 %14
}

define linkonce_odr dso_local i32 @_vsnprintf(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i64, align 8
  %8 = alloca ptr, align 8
  store ptr %3, ptr %5, align 8
  store ptr %2, ptr %6, align 8
  store i64 %1, ptr %7, align 8
  store ptr %0, ptr %8, align 8
  %9 = load ptr, ptr %5, align 8
  %10 = load ptr, ptr %6, align 8
  %11 = load i64, ptr %7, align 8
  %12 = load ptr, ptr %8, align 8
  %13 = call i32 @_vsnprintf_l(ptr noundef %12, i64 noundef %11, ptr noundef %10, ptr noundef null, ptr noundef %9)
  ret i32 %13
}

declare void @llvm.va_start(ptr) #1

define linkonce_odr dso_local i32 @_vsprintf_l(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) #0 comdat {
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

declare void @llvm.va_end(ptr) #1

define linkonce_odr dso_local i32 @_vsnprintf_l(ptr noundef %0, i64 noundef %1, ptr noundef %2, ptr noundef %3, ptr noundef %4) #0 comdat {
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

declare dso_local i32 @__stdio_common_vsprintf(i64 noundef, ptr noundef, i64 noundef, ptr noundef, ptr noundef, ptr noundef) #2

define linkonce_odr dso_local ptr @__local_stdio_printf_options() #0 comdat {
  ret ptr @__local_stdio_printf_options._OptionsStorage
}

attributes #0 = { noinline nounwind optnone uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind willreturn }
attributes #2 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }



define void @stdio.println(ptr %0) {
entry:
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  store ptr %0, ptr %2, align 8
  store i32 0, ptr %3, align 4
  %4 = load ptr, ptr %2, align 8
  br label %PRINTLNLOOP

PRINTLNLOOP: ; Início do loop
  %5 = load i32, ptr %3, align 4
  %6 = getelementptr inbounds i8, ptr %4, i32 %5
  %7 = load i8, ptr %6, align 1
  %8 = icmp eq i8 %7, 0
  br i1 %8, label %PRINTLN.END, label %PRINTLN.BODY

PRINTLN.BODY: ; Corpo do loop
  %9 = sext i8 %7 to i32
  %10 = call i32 @putchar(i32 %9)
  %11 = add i32 %5, 1 ; Incrementa o índice
  store i32 %11, ptr %3, align 4
  br label %PRINTLNLOOP ; Volta para o início do loop

PRINTLN.END: ; Fim do loop
  call i32 @putchar(i32 10) ; Imprime uma nova linha
  ret void
}

define void @heap.dump(ptr %0) {
  call ptr @free(ptr %0)
  ret void
}
