	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"ir.ll"
	.def	person_t.falar;
	.scl	2;
	.type	32;
	.endef
	.globl	person_t.falar                  # -- Begin function person_t.falar
	.p2align	4, 0x90
person_t.falar:                         # @person_t.falar
.seh_proc person_t.falar
# %bb.0:
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%rcx, 64(%rsp)
	movq	$128, 48(%rsp)
	movq	48(%rsp), %rcx
	callq	heap.alloc
	movq	%rax, 56(%rsp)
	movq	56(%rsp), %rcx
	leaq	.L.carla.static.str.0(%rip), %rax
	movq	%rax, 40(%rsp)
	movq	40(%rsp), %rdx
	movq	64(%rsp), %r8
	callq	string.format
	movq	56(%rsp), %rcx
	callq	stdio.println
	movq	56(%rsp), %rcx
	callq	heap.dump
	movb	$0, 39(%rsp)
	movb	39(%rsp), %al
	addq	$72, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.seh_proc main
# %bb.0:                                # %entry
	pushq	%rbp
	.seh_pushreg %rbp
	subq	$32, %rsp
	.seh_stackalloc 32
	leaq	32(%rsp), %rbp
	.seh_setframe %rbp, 32
	.seh_endprologue
	movl	%ecx, -4(%rbp)
	movq	$1, -16(%rbp)
	movq	-16(%rbp), %rax
	movslq	-4(%rbp), %rcx
	cmpq	%rcx, %rax
	sete	%al
	andb	$1, %al
	negb	%al
	movb	%al, -17(%rbp)
	movb	-17(%rbp), %al
	movb	%al, -5(%rbp)
	movb	-5(%rbp), %al
	movb	$0, -18(%rbp)
	cmpb	-18(%rbp), %al
	je	.LBB1_2
# %bb.1:                                # %.l0.carla.that
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rax
	movl	$2, (%rax)
	movl	(%rax), %eax
	movq	%rbp, %rsp
	popq	%rbp
	retq
.LBB1_2:                                # %.l0.carla.else
	jmp	.LBB1_3
.LBB1_3:                                # %.l0.carla.end_root
	movl	$16, %eax
	movq	%rax, -32(%rbp)                 # 8-byte Spill
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rax
	leaq	.L.carla.static.str.1(%rip), %rcx
	movq	%rcx, (%rax)
	movq	(%rax), %rcx
	subq	$32, %rsp
	callq	person_t.falar
	addq	$32, %rsp
                                        # kill: def $cl killed $al
	movq	-32(%rbp), %rax                 # 8-byte Reload
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rax
	movl	$1, (%rax)
	movl	(%rax), %eax
	movq	%rbp, %rsp
	popq	%rbp
	retq
	.seh_endproc
                                        # -- End function
	.def	heap.alloc;
	.scl	2;
	.type	32;
	.endef
	.globl	heap.alloc                      # -- Begin function heap.alloc
	.p2align	4, 0x90
heap.alloc:                             # @heap.alloc
.seh_proc heap.alloc
# %bb.0:
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movq	%rcx, 32(%rsp)
	movq	32(%rsp), %rcx
	callq	malloc
	nop
	addq	$40, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	carla.replace_placeholders;
	.scl	2;
	.type	32;
	.endef
	.globl	carla.replace_placeholders      # -- Begin function carla.replace_placeholders
	.p2align	4, 0x90
carla.replace_placeholders:             # @carla.replace_placeholders
.seh_proc carla.replace_placeholders
# %bb.0:
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%rdx, 48(%rsp)                  # 8-byte Spill
	movq	%rcx, 56(%rsp)                  # 8-byte Spill
	movl	$0, 68(%rsp)
	movl	$0, 64(%rsp)
.LBB3_1:                                # %MAIN_LOOP
                                        # =>This Inner Loop Header: Depth=1
	movq	48(%rsp), %rax                  # 8-byte Reload
	movl	68(%rsp), %ecx
	movl	%ecx, 40(%rsp)                  # 4-byte Spill
	movslq	%ecx, %rcx
	movb	(%rax,%rcx), %al
	movb	%al, 47(%rsp)                   # 1-byte Spill
	cmpb	$0, %al
	je	.LBB3_9
# %bb.2:                                # %PROCESS_CHAR
                                        #   in Loop: Header=BB3_1 Depth=1
	movb	47(%rsp), %al                   # 1-byte Reload
	cmpb	$123, %al
	jne	.LBB3_8
# %bb.3:                                # %HANDLE_PLACEHOLDER
                                        #   in Loop: Header=BB3_1 Depth=1
	movq	48(%rsp), %rax                  # 8-byte Reload
	movl	40(%rsp), %ecx                  # 4-byte Reload
	addl	$1, %ecx
	movl	%ecx, 32(%rsp)                  # 4-byte Spill
	movl	%ecx, 68(%rsp)
	movslq	%ecx, %rcx
	movb	(%rax,%rcx), %al
	cmpb	$115, %al
	sete	%cl
	movb	%cl, 39(%rsp)                   # 1-byte Spill
	cmpb	$100, %al
	je	.LBB3_5
# %bb.4:                                # %CHECK_S
                                        #   in Loop: Header=BB3_1 Depth=1
	movb	39(%rsp), %al                   # 1-byte Reload
	testb	$1, %al
	jne	.LBB3_6
	jmp	.LBB3_7
.LBB3_5:                                # %REPLACE_D
                                        #   in Loop: Header=BB3_1 Depth=1
	movq	56(%rsp), %rcx                  # 8-byte Reload
	leaq	64(%rsp), %rdx
	movl	$37, %r8d
	callq	write_to_output
	movq	56(%rsp), %rcx                  # 8-byte Reload
	leaq	64(%rsp), %rdx
	movl	$100, %r8d
	callq	write_to_output
	movl	32(%rsp), %eax                  # 4-byte Reload
	addl	$2, %eax
	movl	%eax, 68(%rsp)
	jmp	.LBB3_1
.LBB3_6:                                # %REPLACE_S
                                        #   in Loop: Header=BB3_1 Depth=1
	movq	56(%rsp), %rcx                  # 8-byte Reload
	leaq	64(%rsp), %rdx
	movl	$37, %r8d
	callq	write_to_output
	movq	56(%rsp), %rcx                  # 8-byte Reload
	leaq	64(%rsp), %rdx
	movl	$115, %r8d
	callq	write_to_output
	movl	32(%rsp), %eax                  # 4-byte Reload
	addl	$2, %eax
	movl	%eax, 68(%rsp)
	jmp	.LBB3_1
.LBB3_7:                                # %INVALID_PLACEHOLDER
                                        #   in Loop: Header=BB3_1 Depth=1
	movl	32(%rsp), %eax                  # 4-byte Reload
	addl	$1, %eax
	movl	%eax, 68(%rsp)
	jmp	.LBB3_1
.LBB3_8:                                # %COPY_CHAR
                                        #   in Loop: Header=BB3_1 Depth=1
	movb	47(%rsp), %r8b                  # 1-byte Reload
	movq	56(%rsp), %rcx                  # 8-byte Reload
	leaq	64(%rsp), %rdx
	callq	write_to_output
	movl	40(%rsp), %eax                  # 4-byte Reload
	addl	$1, %eax
	movl	%eax, 68(%rsp)
	jmp	.LBB3_1
.LBB3_9:                                # %END
	movq	56(%rsp), %rcx                  # 8-byte Reload
	leaq	64(%rsp), %rdx
	xorl	%r8d, %r8d
	callq	write_to_output
	nop
	addq	$72, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	write_to_output;
	.scl	2;
	.type	32;
	.endef
	.globl	write_to_output                 # -- Begin function write_to_output
	.p2align	4, 0x90
write_to_output:                        # @write_to_output
# %bb.0:
	movb	%r8b, %r9b
	movl	(%rdx), %eax
	movslq	%eax, %r8
	movb	%r9b, (%rcx,%r8)
	addl	$1, %eax
	movl	%eax, (%rdx)
	retq
                                        # -- End function
	.def	string.format;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,string.format
	.globl	string.format                   # -- Begin function string.format
	.p2align	4, 0x90
string.format:                          # @string.format
.seh_proc string.format
# %bb.0:
	subq	$88, %rsp
	.seh_stackalloc 88
	.seh_endprologue
	movq	%rdx, 32(%rsp)                  # 8-byte Spill
	movq	%rcx, 48(%rsp)                  # 8-byte Spill
	movq	%r9, 120(%rsp)
	movq	%r8, 112(%rsp)
	movl	$1048576, %ecx                  # imm = 0x100000
	callq	malloc
	movq	32(%rsp), %rdx                  # 8-byte Reload
	movq	%rax, %rcx
	movq	%rcx, 40(%rsp)                  # 8-byte Spill
	callq	carla.replace_placeholders
	movq	40(%rsp), %rax                  # 8-byte Reload
	movq	48(%rsp), %rcx                  # 8-byte Reload
	movq	%rax, 80(%rsp)
	movq	%rcx, 72(%rsp)
	leaq	112(%rsp), %rax
	movq	%rax, 56(%rsp)
	movq	56(%rsp), %r9
	movq	80(%rsp), %rdx
	movq	72(%rsp), %rcx
	xorl	%eax, %eax
	movl	%eax, %r8d
	callq	_vsprintf_l
	movl	%eax, 68(%rsp)
	movl	68(%rsp), %eax
	addq	$88, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	vsprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,vsprintf
	.globl	vsprintf                        # -- Begin function vsprintf
	.p2align	4, 0x90
vsprintf:                               # @vsprintf
.seh_proc vsprintf
# %bb.0:
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%r8, 64(%rsp)
	movq	%rdx, 56(%rsp)
	movq	%rcx, 48(%rsp)
	movq	64(%rsp), %rax
	movq	56(%rsp), %r8
	movq	48(%rsp), %rcx
	movq	$-1, %rdx
	xorl	%r9d, %r9d
                                        # kill: def $r9 killed $r9d
	movq	%rax, 32(%rsp)
	callq	_vsnprintf_l
	nop
	addq	$72, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	_snprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_snprintf
	.globl	_snprintf                       # -- Begin function _snprintf
	.p2align	4, 0x90
_snprintf:                              # @_snprintf
.seh_proc _snprintf
# %bb.0:
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%r9, 104(%rsp)
	movq	%r8, 64(%rsp)
	movq	%rdx, 56(%rsp)
	movq	%rcx, 48(%rsp)
	leaq	104(%rsp), %rax
	movq	%rax, 32(%rsp)
	movq	32(%rsp), %r9
	movq	64(%rsp), %r8
	movq	56(%rsp), %rdx
	movq	48(%rsp), %rcx
	callq	_vsnprintf
	movl	%eax, 44(%rsp)
	movl	44(%rsp), %eax
	addq	$72, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	_vsnprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_vsnprintf
	.globl	_vsnprintf                      # -- Begin function _vsnprintf
	.p2align	4, 0x90
_vsnprintf:                             # @_vsnprintf
.seh_proc _vsnprintf
# %bb.0:
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%r9, 64(%rsp)
	movq	%r8, 56(%rsp)
	movq	%rdx, 48(%rsp)
	movq	%rcx, 40(%rsp)
	movq	64(%rsp), %rax
	movq	56(%rsp), %r8
	movq	48(%rsp), %rdx
	movq	40(%rsp), %rcx
	xorl	%r9d, %r9d
                                        # kill: def $r9 killed $r9d
	movq	%rax, 32(%rsp)
	callq	_vsnprintf_l
	nop
	addq	$72, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	_vsprintf_l;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_vsprintf_l
	.globl	_vsprintf_l                     # -- Begin function _vsprintf_l
	.p2align	4, 0x90
_vsprintf_l:                            # @_vsprintf_l
.seh_proc _vsprintf_l
# %bb.0:
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%r9, 64(%rsp)
	movq	%r8, 56(%rsp)
	movq	%rdx, 48(%rsp)
	movq	%rcx, 40(%rsp)
	movq	64(%rsp), %rax
	movq	56(%rsp), %r9
	movq	48(%rsp), %r8
	movq	40(%rsp), %rcx
	movq	$-1, %rdx
	movq	%rax, 32(%rsp)
	callq	_vsnprintf_l
	nop
	addq	$72, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	_vsnprintf_l;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_vsnprintf_l
	.globl	_vsnprintf_l                    # -- Begin function _vsnprintf_l
	.p2align	4, 0x90
_vsnprintf_l:                           # @_vsnprintf_l
.seh_proc _vsnprintf_l
# %bb.0:
	subq	$136, %rsp
	.seh_stackalloc 136
	.seh_endprologue
	movq	176(%rsp), %rax
	movq	%r9, 128(%rsp)
	movq	%r8, 120(%rsp)
	movq	%rdx, 112(%rsp)
	movq	%rcx, 104(%rsp)
	movq	176(%rsp), %rax
	movq	%rax, 88(%rsp)                  # 8-byte Spill
	movq	128(%rsp), %rax
	movq	%rax, 80(%rsp)                  # 8-byte Spill
	movq	120(%rsp), %rax
	movq	%rax, 72(%rsp)                  # 8-byte Spill
	movq	112(%rsp), %rax
	movq	%rax, 64(%rsp)                  # 8-byte Spill
	movq	104(%rsp), %rax
	movq	%rax, 56(%rsp)                  # 8-byte Spill
	callq	__local_stdio_printf_options
	movq	56(%rsp), %rdx                  # 8-byte Reload
	movq	64(%rsp), %r8                   # 8-byte Reload
	movq	72(%rsp), %r9                   # 8-byte Reload
	movq	80(%rsp), %r10                  # 8-byte Reload
	movq	%rax, %rcx
	movq	88(%rsp), %rax                  # 8-byte Reload
	movq	(%rcx), %rcx
	orq	$1, %rcx
	movq	%r10, 32(%rsp)
	movq	%rax, 40(%rsp)
	callq	__stdio_common_vsprintf
	movl	%eax, 100(%rsp)
	cmpl	$0, 100(%rsp)
	jge	.LBB10_2
# %bb.1:
	movl	$4294967295, %eax               # imm = 0xFFFFFFFF
	movl	%eax, 52(%rsp)                  # 4-byte Spill
	jmp	.LBB10_3
.LBB10_2:
	movl	100(%rsp), %eax
	movl	%eax, 52(%rsp)                  # 4-byte Spill
.LBB10_3:
	movl	52(%rsp), %eax                  # 4-byte Reload
	addq	$136, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	__local_stdio_printf_options;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,__local_stdio_printf_options
	.globl	__local_stdio_printf_options    # -- Begin function __local_stdio_printf_options
	.p2align	4, 0x90
__local_stdio_printf_options:           # @__local_stdio_printf_options
# %bb.0:
	leaq	__local_stdio_printf_options._OptionsStorage(%rip), %rax
	retq
                                        # -- End function
	.def	stdio.println;
	.scl	2;
	.type	32;
	.endef
	.text
	.globl	stdio.println                   # -- Begin function stdio.println
	.p2align	4, 0x90
stdio.println:                          # @stdio.println
.seh_proc stdio.println
# %bb.0:                                # %entry
	subq	$72, %rsp
	.seh_stackalloc 72
	.seh_endprologue
	movq	%rcx, 64(%rsp)
	movl	$0, 60(%rsp)
	movq	64(%rsp), %rax
	movq	%rax, 48(%rsp)                  # 8-byte Spill
.LBB12_1:                               # %PRINTLNLOOP
                                        # =>This Inner Loop Header: Depth=1
	movq	48(%rsp), %rax                  # 8-byte Reload
	movl	60(%rsp), %ecx
	movl	%ecx, 40(%rsp)                  # 4-byte Spill
	movslq	%ecx, %rcx
	movb	(%rax,%rcx), %al
	movb	%al, 47(%rsp)                   # 1-byte Spill
	cmpb	$0, %al
	je	.LBB12_3
# %bb.2:                                # %PRINTLN.BODY
                                        #   in Loop: Header=BB12_1 Depth=1
	movb	47(%rsp), %al                   # 1-byte Reload
	movsbl	%al, %ecx
	callq	putchar
                                        # kill: def $ecx killed $eax
	movl	40(%rsp), %eax                  # 4-byte Reload
	addl	$1, %eax
	movl	%eax, 60(%rsp)
	jmp	.LBB12_1
.LBB12_3:                               # %PRINTLN.END
	movl	$10, %ecx
	callq	putchar
	nop
	addq	$72, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.def	heap.dump;
	.scl	2;
	.type	32;
	.endef
	.globl	heap.dump                       # -- Begin function heap.dump
	.p2align	4, 0x90
heap.dump:                              # @heap.dump
.seh_proc heap.dump
# %bb.0:
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	callq	free
	nop
	addq	$40, %rsp
	retq
	.seh_endproc
                                        # -- End function
	.section	.rdata,"dr"
.L.carla.static.str.0:                  # @.carla.static.str.0
	.asciz	"A pessoa esta falando: {s}"

.L.carla.static.str.1:                  # @.carla.static.str.1
	.asciz	"ola"

	.lcomm	__local_stdio_printf_options._OptionsStorage,8,8 # @__local_stdio_printf_options._OptionsStorage
	.addrsig
	.addrsig_sym person_t.falar
	.addrsig_sym putchar
	.addrsig_sym malloc
	.addrsig_sym free
	.addrsig_sym heap.alloc
	.addrsig_sym carla.replace_placeholders
	.addrsig_sym write_to_output
	.addrsig_sym string.format
	.addrsig_sym _vsnprintf
	.addrsig_sym _vsprintf_l
	.addrsig_sym _vsnprintf_l
	.addrsig_sym __stdio_common_vsprintf
	.addrsig_sym __local_stdio_printf_options
	.addrsig_sym stdio.println
	.addrsig_sym heap.dump
	.addrsig_sym .L.carla.static.str.0
	.addrsig_sym .L.carla.static.str.1
	.addrsig_sym __local_stdio_printf_options._OptionsStorage
