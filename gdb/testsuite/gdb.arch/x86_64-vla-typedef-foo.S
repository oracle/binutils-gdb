	.file	"x86_64-vla-typedef.c"
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.text
.Ltext0:
.globl foo
	.type	foo, @function
foo:
.LFB2:
	.file 1 "x86_64-vla-typedef.c"
	.loc 1 22 0
	pushq	%rbp
.LCFI0:
	movq	%rsp, %rbp
.LCFI1:
	subq	$64, %rsp
.LCFI2:
	movl	%edi, -36(%rbp)
	.loc 1 22 0
	movq	%rsp, %rax
	movq	%rax, -48(%rbp)
	.loc 1 23 0
	movl	-36(%rbp), %edx
	movslq	%edx,%rax
	subq	$1, %rax
	movq	%rax, -24(%rbp)
	.loc 1 24 0
	movslq	%edx,%rax
	addq	$15, %rax
	addq	$15, %rax
	shrq	$4, %rax
	salq	$4, %rax
	subq	%rax, %rsp
	movq	%rsp, -56(%rbp)
	movq	-56(%rbp), %rax
	addq	$15, %rax
	shrq	$4, %rax
	salq	$4, %rax
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, -16(%rbp)
	.loc 1 27 0
	movl	$0, -4(%rbp)
	jmp	.L2
.L3:
	.loc 1 28 0
	movl	-4(%rbp), %esi
	movl	-4(%rbp), %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movslq	%esi,%rax
	movb	%cl, (%rdx,%rax)
	.loc 1 27 0
	addl	$1, -4(%rbp)
.L2:
	movl	-4(%rbp), %eax
	cmpl	-36(%rbp), %eax
	jl	.L3
	.loc 1 30 0
	.globl	break_here
break_here:
	movq	-16(%rbp), %rax
	movb	$0, (%rax)
	movq	-48(%rbp), %rsp
	.loc 1 31 0
	leave
	ret
.LFE2:
	.size	foo, .-foo
	.section	.debug_frame,"",@progbits
.Lframe0:
	.long	.LECIE0-.LSCIE0
.LSCIE0:
	.long	0xffffffff
	.byte	0x1
	.string	""
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 8
.LECIE0:
.LSFDE0:
	.long	.LEFDE0-.LASFDE0
.LASFDE0:
	.long	.Lframe0
	.quad	.LFB2
	.quad	.LFE2-.LFB2
	.byte	0x4
	.long	.LCFI0-.LFB2
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI1-.LCFI0
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE0:
	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	"zR"
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.uleb128 0x1
	.byte	0x3
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 8
.LECIE1:
.LSFDE1:
	.long	.LEFDE1-.LASFDE1
.LASFDE1:
	.long	.LASFDE1-.Lframe1
	.long	.LFB2
	.long	.LFE2-.LFB2
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI0-.LFB2
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI1-.LCFI0
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE1:
	.text
.Letext0:
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.quad	.LFB2-.Ltext0
	.quad	.LCFI0-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI0-.Ltext0
	.quad	.LCFI1-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI1-.Ltext0
	.quad	.LFE2-.Ltext0
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	0x0
	.quad	0x0
	.section	.debug_info
	.long	.Ldebug_end - .Ldebug_start
.Ldebug_start:
	.value	0x2
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF2
	.byte	0x1
	.long	.LASF3
	.long	.LASF4
	.quad	.Ltext0
	.quad	.Letext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.string	"foo"
	.byte	0x1
	.byte	0x16
	.byte	0x1
	.quad	.LFB2
	.quad	.LFE2
	.long	.LLST0
	.long	0x83
	.uleb128 0x3
	.long	.LASF5
	.byte	0x1
	.byte	0x15
	.long	0x83
	.byte	0x2
	.byte	0x91
	.sleb128 -52
.Ltag_typedef:
	.uleb128 0x4
	.long	.LASF6
	.byte	0x1
	.byte	0x17
	.long	.Ltag_array_type - .debug_info
	.uleb128 0x5	/* Abbrev Number: 5 (DW_TAG_variable) */
	.long	.LASF0
	.byte	0x1
	.byte	0x18
#if 1
	.long	.Ltag_typedef - .debug_info
#else
	/* Debugging only: Skip the typedef indirection.  */
	.long	.Ltag_array_type - .debug_info
#endif
	/* DW_AT_location: DW_FORM_block1: start */
	.byte	0x3
	.byte	0x91
	.sleb128 -32
#if 0
	.byte	0x6	/* DW_OP_deref */
#else
	.byte	0x96	/* DW_OP_nop */
#endif
	/* DW_AT_location: DW_FORM_block1: end */
	.uleb128 0x6
	.string	"i"
	.byte	0x1
	.byte	0x19
	.long	0x83
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0x0
	.uleb128 0x7
	.byte	0x4
	.byte	0x5
	.string	"int"
.Ltag_array_type:
	.uleb128 0x8	/* Abbrev Number: 8 (DW_TAG_array_type) */
	.long	0xa0 + (2f - 1f)	/* DW_AT_type: DW_FORM_ref4 */
	.long	0x9d + (2f - 1f)	/* DW_AT_sibling: DW_FORM_ref4 */
1:	/* DW_AT_data_location: DW_FORM_block1: start */
	.byte	2f - 3f	/* length */
3:
	.byte	0x97	/* DW_OP_push_object_address */
	.byte	0x6	/* DW_OP_deref */
2:	/* DW_AT_data_location: DW_FORM_block1: end */
	.uleb128 0x9
	.long	0x9d + (2b - 1b)	/* DW_AT_type: DW_FORM_ref4 */
	.byte	0x3
	.byte	0x91
	.sleb128 -40
	.byte	0x6
	.byte	0x0
	.uleb128 0xa
	.byte	0x8
	.byte	0x7
	.uleb128 0xb
	.byte	0x1
	.byte	0x6
	.long	.LASF1
	.byte	0x0
.Ldebug_end:
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x8	/* Abbrev Number: 8 (DW_TAG_array_type) */
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49	/* DW_AT_type */
	.uleb128 0x13	/* DW_FORM_ref4 */
	.uleb128 0x1	/* DW_AT_sibling */
	.uleb128 0x13	/* DW_FORM_ref4 */
	.uleb128 0x50	/* DW_AT_data_location */
	.uleb128 0xa	/* DW_FORM_block1 */
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49	/* DW_AT_type */
	.uleb128 0x13	/* DW_FORM_ref4 */
	.uleb128 0x2f
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",@progbits
	.long	0x16
	.value	0x2
	.long	.Ldebug_info0
	.long	0xa8
	.long	0x2d
	.string	"foo"
	.long	0x0
	.section	.debug_aranges,"",@progbits
	.long	0x2c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0x0
	.value	0x0
	.value	0x0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	0x0
	.quad	0x0
	.section	.debug_str,"MS",@progbits,1
.LASF0:
	.string	"array"
.LASF5:
	.string	"size"
.LASF3:
	.string	"x86_64-vla-typedef.c"
.LASF6:
	.string	"array_t"
.LASF1:
	.string	"char"
.LASF4:
	.string	"gdb.arch"
.LASF2:
	.string	"GNU C 4.3.2 20081105 (Red Hat 4.3.2-7)"
	.ident	"GCC: (GNU) 4.3.2 20081105 (Red Hat 4.3.2-7)"
	.section	.note.GNU-stack,"",@progbits
