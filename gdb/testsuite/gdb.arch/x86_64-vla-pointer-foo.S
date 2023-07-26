	.file	"x86_64-vla-pointer.c"
	.text
.Ltext0:
	.globl	foo
	.type	foo, @function
foo:
.LFB0:
	.file 1 "gdb.arch/x86_64-vla-pointer.c"
	# gdb.arch/x86_64-vla-pointer.c:22
	.loc 1 22 0
	.cfi_startproc
# BLOCK 2 seq:0
# PRED: ENTRY (FALLTHRU)
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -24
	movl	%edi, -52(%rbp)
	# gdb.arch/x86_64-vla-pointer.c:22
	.loc 1 22 0
	movq	%rsp, %rax
	movq	%rax, %rsi
	# gdb.arch/x86_64-vla-pointer.c:23
	.loc 1 23 0
	movl	-52(%rbp), %eax
	movslq	%eax, %rdx
	subq	$1, %rdx
	movq	%rdx, -32(%rbp)
	movslq	%eax, %rdx
	movq	%rdx, %r8
	movl	$0, %r9d
	# gdb.arch/x86_64-vla-pointer.c:24
	.loc 1 24 0
	movslq	%eax, %rdx
	movq	%rdx, %rcx
	movl	$0, %ebx
	cltq
	movl	$16, %edx
	subq	$1, %rdx
	addq	%rdx, %rax
	movl	$16, %ebx
	movl	$0, %edx
	divq	%rbx
	imulq	$16, %rax, %rax
	subq	%rax, %rsp
	movq	%rsp, %rax
	addq	$0, %rax
	movq	%rax, -40(%rbp)
	# gdb.arch/x86_64-vla-pointer.c:27
	.loc 1 27 0
	movl	$0, -20(%rbp)
# SUCC: 4 [100.0%] 
	jmp	.L2
# BLOCK 3 seq:1
# PRED: 4
.L3:
	# gdb.arch/x86_64-vla-pointer.c:28
	.loc 1 28 0 discriminator 3
	movl	-20(%rbp), %eax
	movl	%eax, %ecx
	movq	-40(%rbp), %rdx
	movl	-20(%rbp), %eax
	cltq
	movb	%cl, (%rdx,%rax)
# SUCC: 4 (FALLTHRU,DFS_BACK)
	# gdb.arch/x86_64-vla-pointer.c:27
	.loc 1 27 0 discriminator 3
	addl	$1, -20(%rbp)
# BLOCK 4 seq:2
# PRED: 3 (FALLTHRU,DFS_BACK) 2 [100.0%] 
.L2:
	# gdb.arch/x86_64-vla-pointer.c:27
	.loc 1 27 0 is_stmt 0 discriminator 1
	movl	-20(%rbp), %eax
	cmpl	-52(%rbp), %eax
# SUCC: 3 5 (FALLTHRU)
	jl	.L3
# BLOCK 5 seq:3
# PRED: 4 (FALLTHRU)
	# gdb.arch/x86_64-vla-pointer.c:30
	.loc 1 30 0 is_stmt 1
	movq	-40(%rbp), %rax
	movb	$0, (%rax)
	movq	%rsi, %rsp
	# gdb.arch/x86_64-vla-pointer.c:31
	.loc 1 31 0
	nop
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
# SUCC: EXIT [100.0%] 
	ret
	.cfi_endproc
.LFE0:
	.size	foo, .-foo
.Letext0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0xa5	# Length of Compilation Unit Info
	.value	0x4	# DWARF version number
	.long	.Ldebug_abbrev0	# Offset Into Abbrev. Section
	.byte	0x8	# Pointer Size (in bytes)
	.uleb128 0x1	# (DIE (0xb) DW_TAG_compile_unit)
	.long	.LASF3	# DW_AT_producer: "GNU C11 6.2.1 20160916 (Red Hat 6.2.1-2) -mtune=generic -march=x86-64 -g"
	.byte	0xc	# DW_AT_language
	.long	.LASF4	# DW_AT_name: "gdb.arch/x86_64-vla-pointer.c"
	.long	.LASF5	# DW_AT_comp_dir: "/home/jkratoch/redhat/fedora/gdb/master/gdb-7.12/gdb/testsuite"
	.quad	.Ltext0	# DW_AT_low_pc
	.quad	.Letext0-.Ltext0	# DW_AT_high_pc
	.long	.Ldebug_line0	# DW_AT_stmt_list
	.uleb128 0x2	# (DIE (0x2d) DW_TAG_subprogram)
			# DW_AT_external
	.ascii "foo\0"	# DW_AT_name
	.byte	0x1	# DW_AT_decl_file (gdb.arch/x86_64-vla-pointer.c)
	.byte	0x15	# DW_AT_decl_line
			# DW_AT_prototyped
	.quad	.LFB0	# DW_AT_low_pc
	.quad	.LFE0-.LFB0	# DW_AT_high_pc
	.uleb128 0x1	# DW_AT_frame_base
	.byte	0x9c	# DW_OP_call_frame_cfa
			# DW_AT_GNU_all_call_sites
	.long	0x80	# DW_AT_sibling
	.uleb128 0x3	# (DIE (0x4a) DW_TAG_formal_parameter)
	.long	.LASF6	# DW_AT_name: "size"
	.byte	0x1	# DW_AT_decl_file (gdb.arch/x86_64-vla-pointer.c)
	.byte	0x15	# DW_AT_decl_line
	.long	0x80	# DW_AT_type
	.uleb128 0x3	# DW_AT_location
	.byte	0x91	# DW_OP_fbreg
	.sleb128 -68
	.uleb128 0x4	# (DIE (0x59) DW_TAG_typedef)
	.long	.LASF7	# DW_AT_name: "array_t"
	.byte	0x1	# DW_AT_decl_file (gdb.arch/x86_64-vla-pointer.c)
	.byte	0x17	# DW_AT_decl_line
	.long	0x87	# DW_AT_type
	.uleb128 0x5	# (DIE (0x64) DW_TAG_variable)
	.long	.LASF0	# DW_AT_name: "array"
	.byte	0x1	# DW_AT_decl_file (gdb.arch/x86_64-vla-pointer.c)
	.byte	0x18	# DW_AT_decl_line
	.long	0x59	# DW_AT_type
	.uleb128 0x3	# DW_AT_location
	.byte	0x91	# DW_OP_fbreg
	.sleb128 -56
	.byte	0x6	# DW_OP_deref
	.uleb128 0x6	# (DIE (0x73) DW_TAG_variable)
	.ascii "i\0"	# DW_AT_name
	.byte	0x1	# DW_AT_decl_file (gdb.arch/x86_64-vla-pointer.c)
	.byte	0x19	# DW_AT_decl_line
	.long	0x80	# DW_AT_type
	.uleb128 0x2	# DW_AT_location
	.byte	0x91	# DW_OP_fbreg
	.sleb128 -36
	.byte	0	# end of children of DIE 0x2d
	.uleb128 0x7	# (DIE (0x80) DW_TAG_base_type)
	.byte	0x4	# DW_AT_byte_size
	.byte	0x5	# DW_AT_encoding
	.ascii "int\0"	# DW_AT_name
	.uleb128 0x8	# (DIE (0x87) DW_TAG_array_type)
	.long	0xa1	# DW_AT_type
	.long	0x9a	# DW_AT_sibling
	.uleb128 0x9	# (DIE (0x90) DW_TAG_subrange_type)
	.long	0x9a	# DW_AT_type
	.uleb128 0x3	# DW_AT_upper_bound
	.byte	0x91	# DW_OP_fbreg
	.sleb128 -48
	.byte	0x6	# DW_OP_deref
	.byte	0	# end of children of DIE 0x87
	.uleb128 0xa	# (DIE (0x9a) DW_TAG_base_type)
	.byte	0x8	# DW_AT_byte_size
	.byte	0x7	# DW_AT_encoding
	.long	.LASF1	# DW_AT_name: "sizetype"
	.uleb128 0xa	# (DIE (0xa1) DW_TAG_base_type)
	.byte	0x1	# DW_AT_byte_size
	.byte	0x6	# DW_AT_encoding
	.long	.LASF2	# DW_AT_name: "char"
	.byte	0	# end of children of DIE 0xb
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1	# (abbrev code)
	.uleb128 0x11	# (TAG: DW_TAG_compile_unit)
	.byte	0x1	# DW_children_yes
	.uleb128 0x25	# (DW_AT_producer)
	.uleb128 0xe	# (DW_FORM_strp)
	.uleb128 0x13	# (DW_AT_language)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x3	# (DW_AT_name)
	.uleb128 0xe	# (DW_FORM_strp)
	.uleb128 0x1b	# (DW_AT_comp_dir)
	.uleb128 0xe	# (DW_FORM_strp)
	.uleb128 0x11	# (DW_AT_low_pc)
	.uleb128 0x1	# (DW_FORM_addr)
	.uleb128 0x12	# (DW_AT_high_pc)
	.uleb128 0x7	# (DW_FORM_data8)
	.uleb128 0x10	# (DW_AT_stmt_list)
	.uleb128 0x17	# (DW_FORM_sec_offset)
	.byte	0
	.byte	0
	.uleb128 0x2	# (abbrev code)
	.uleb128 0x2e	# (TAG: DW_TAG_subprogram)
	.byte	0x1	# DW_children_yes
	.uleb128 0x3f	# (DW_AT_external)
	.uleb128 0x19	# (DW_FORM_flag_present)
	.uleb128 0x3	# (DW_AT_name)
	.uleb128 0x8	# (DW_FORM_string)
	.uleb128 0x3a	# (DW_AT_decl_file)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x3b	# (DW_AT_decl_line)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x27	# (DW_AT_prototyped)
	.uleb128 0x19	# (DW_FORM_flag_present)
	.uleb128 0x11	# (DW_AT_low_pc)
	.uleb128 0x1	# (DW_FORM_addr)
	.uleb128 0x12	# (DW_AT_high_pc)
	.uleb128 0x7	# (DW_FORM_data8)
	.uleb128 0x40	# (DW_AT_frame_base)
	.uleb128 0x18	# (DW_FORM_exprloc)
	.uleb128 0x2117	# (DW_AT_GNU_all_call_sites)
	.uleb128 0x19	# (DW_FORM_flag_present)
	.uleb128 0x1	# (DW_AT_sibling)
	.uleb128 0x13	# (DW_FORM_ref4)
	.byte	0
	.byte	0
	.uleb128 0x3	# (abbrev code)
	.uleb128 0x5	# (TAG: DW_TAG_formal_parameter)
	.byte	0	# DW_children_no
	.uleb128 0x3	# (DW_AT_name)
	.uleb128 0xe	# (DW_FORM_strp)
	.uleb128 0x3a	# (DW_AT_decl_file)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x3b	# (DW_AT_decl_line)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x49	# (DW_AT_type)
	.uleb128 0x13	# (DW_FORM_ref4)
	.uleb128 0x2	# (DW_AT_location)
	.uleb128 0x18	# (DW_FORM_exprloc)
	.byte	0
	.byte	0
	.uleb128 0x4	# (abbrev code)
	.uleb128 0x16	# (TAG: DW_TAG_typedef)
	.byte	0	# DW_children_no
	.uleb128 0x3	# (DW_AT_name)
	.uleb128 0xe	# (DW_FORM_strp)
	.uleb128 0x3a	# (DW_AT_decl_file)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x3b	# (DW_AT_decl_line)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x49	# (DW_AT_type)
	.uleb128 0x13	# (DW_FORM_ref4)
	.byte	0
	.byte	0
	.uleb128 0x5	# (abbrev code)
	.uleb128 0x34	# (TAG: DW_TAG_variable)
	.byte	0	# DW_children_no
	.uleb128 0x3	# (DW_AT_name)
	.uleb128 0xe	# (DW_FORM_strp)
	.uleb128 0x3a	# (DW_AT_decl_file)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x3b	# (DW_AT_decl_line)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x49	# (DW_AT_type)
	.uleb128 0x13	# (DW_FORM_ref4)
	.uleb128 0x2	# (DW_AT_location)
	.uleb128 0x18	# (DW_FORM_exprloc)
	.byte	0
	.byte	0
	.uleb128 0x6	# (abbrev code)
	.uleb128 0x34	# (TAG: DW_TAG_variable)
	.byte	0	# DW_children_no
	.uleb128 0x3	# (DW_AT_name)
	.uleb128 0x8	# (DW_FORM_string)
	.uleb128 0x3a	# (DW_AT_decl_file)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x3b	# (DW_AT_decl_line)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x49	# (DW_AT_type)
	.uleb128 0x13	# (DW_FORM_ref4)
	.uleb128 0x2	# (DW_AT_location)
	.uleb128 0x18	# (DW_FORM_exprloc)
	.byte	0
	.byte	0
	.uleb128 0x7	# (abbrev code)
	.uleb128 0x24	# (TAG: DW_TAG_base_type)
	.byte	0	# DW_children_no
	.uleb128 0xb	# (DW_AT_byte_size)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x3e	# (DW_AT_encoding)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x3	# (DW_AT_name)
	.uleb128 0x8	# (DW_FORM_string)
	.byte	0
	.byte	0
	.uleb128 0x8	# (abbrev code)
	.uleb128 0x1	# (TAG: DW_TAG_array_type)
	.byte	0x1	# DW_children_yes
	.uleb128 0x49	# (DW_AT_type)
	.uleb128 0x13	# (DW_FORM_ref4)
	.uleb128 0x1	# (DW_AT_sibling)
	.uleb128 0x13	# (DW_FORM_ref4)
	.byte	0
	.byte	0
	.uleb128 0x9	# (abbrev code)
	.uleb128 0x21	# (TAG: DW_TAG_subrange_type)
	.byte	0	# DW_children_no
	.uleb128 0x49	# (DW_AT_type)
	.uleb128 0x13	# (DW_FORM_ref4)
	.uleb128 0x2f	# (DW_AT_upper_bound)
	.uleb128 0x18	# (DW_FORM_exprloc)
	.byte	0
	.byte	0
	.uleb128 0xa	# (abbrev code)
	.uleb128 0x24	# (TAG: DW_TAG_base_type)
	.byte	0	# DW_children_no
	.uleb128 0xb	# (DW_AT_byte_size)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x3e	# (DW_AT_encoding)
	.uleb128 0xb	# (DW_FORM_data1)
	.uleb128 0x3	# (DW_AT_name)
	.uleb128 0xe	# (DW_FORM_strp)
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.long	0x2c	# Length of Address Ranges Info
	.value	0x2	# DWARF Version
	.long	.Ldebug_info0	# Offset of Compilation Unit Info
	.byte	0x8	# Size of Address
	.byte	0	# Size of Segment Descriptor
	.value	0	# Pad to 16 byte boundary
	.value	0
	.quad	.Ltext0	# Address
	.quad	.Letext0-.Ltext0	# Length
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF4:
	.string	"gdb.arch/x86_64-vla-pointer.c"
.LASF7:
	.string	"array_t"
.LASF3:
	.string	"GNU C11 6.2.1 20160916 (Red Hat 6.2.1-2) -mtune=generic -march=x86-64 -g"
.LASF2:
	.string	"char"
.LASF1:
	.string	"sizetype"
.LASF5:
	.string	"/home/jkratoch/redhat/fedora/gdb/master/gdb-7.12/gdb/testsuite"
.LASF6:
	.string	"size"
.LASF0:
	.string	"array"
	.ident	"GCC: (GNU) 6.2.1 20160916 (Red Hat 6.2.1-2)"
	.section	.note.GNU-stack,"",@progbits
