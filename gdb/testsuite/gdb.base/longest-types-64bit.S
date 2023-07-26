/* This testcase is part of GDB, the GNU debugger.

   Copyright 2012 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

	.file	"longest-types-64bit.c"
	.text
.Ltext0:
	.globl main
main:
	.comm	f,8,8
.Letext0:
	.file 1 "gdb.base/longest-types-64bit.c"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.4byte	0x9a	/* Length of Compilation Unit Info */
	.2byte	0x2	/* DWARF version number */
	.4byte	.Ldebug_abbrev0	/* Offset Into Abbrev. Section */
	.byte	0x8	/* Pointer Size (in bytes) */
	.uleb128 0x1	/* (DIE (0xb) DW_TAG_compile_unit) */
	.4byte	.LASF3	/* DW_AT_producer: "GNU C 4.7.3 20121109 (prerelease)" */
	.byte	0x1	/* DW_AT_language */
	.4byte	.LASF4	/* DW_AT_name: "gdb.base/longest-types-64bit.c" */
	.4byte	.LASF5	/* DW_AT_comp_dir: "" */
	.4byte	.Ldebug_line0	/* DW_AT_stmt_list */
	.uleb128 0x2	/* (DIE (0x1d) DW_TAG_structure_type) */
	.ascii "foo\0"	/* DW_AT_name */
	.4byte	0xff000002	/* DW_AT_byte_size */
	.byte	0x1	/* DW_AT_decl_file (gdb.base/longest-types-64bit.c) */
	.byte	0x12	/* DW_AT_decl_line */
	.4byte	0x4e	/* DW_AT_sibling */
	.uleb128 0x3	/* (DIE (0x2c) DW_TAG_member) */
	.ascii "buf\0"	/* DW_AT_name */
	.byte	0x1	/* DW_AT_decl_file (gdb.base/longest-types-64bit.c) */
	.byte	0x14	/* DW_AT_decl_line */
	.4byte	0x4e	/* DW_AT_type */
	.byte	0x2	/* DW_AT_data_member_location */
	.byte	0x23	/* DW_OP_plus_uconst */
	.uleb128 0
	.uleb128 0x4	/* (DIE (0x3a) DW_TAG_member) */
	.4byte	.LASF0	/* DW_AT_name: "buf2" */
	.byte	0x1	/* DW_AT_decl_file (gdb.base/longest-types-64bit.c) */
	.byte	0x15	/* DW_AT_decl_line */
	.4byte	0x73	/* DW_AT_type */
	.byte	0x7	/* DW_AT_data_member_location */
	.byte	0x23	/* DW_OP_plus_uconst */
	.uleb128 0xffff000000
	.byte	0	/* end of children of DIE 0x1d */
	.uleb128 0x5	/* (DIE (0x4e) DW_TAG_array_type) */
	.4byte	0x6c	/* DW_AT_type */
	.4byte	0x65	/* DW_AT_sibling */
	.uleb128 0x6	/* (DIE (0x57) DW_TAG_subrange_type) */
	.4byte	0x65	/* DW_AT_type */
	.quad	0xfffeffffff	/* DW_AT_upper_bound */
	.byte	0	/* end of children of DIE 0x4e */
	.uleb128 0x7	/* (DIE (0x65) DW_TAG_base_type) */
	.byte	0x8	/* DW_AT_byte_size */
	.byte	0x7	/* DW_AT_encoding */
	.4byte	.LASF1	/* DW_AT_name: "sizetype" */
	.uleb128 0x7	/* (DIE (0x6c) DW_TAG_base_type) */
	.byte	0x1	/* DW_AT_byte_size */
	.byte	0x6	/* DW_AT_encoding */
	.4byte	.LASF2	/* DW_AT_name: "char" */
	.uleb128 0x5	/* (DIE (0x73) DW_TAG_array_type) */
	.4byte	0x6c	/* DW_AT_type */
	.4byte	0x83	/* DW_AT_sibling */
	.uleb128 0x8	/* (DIE (0x7c) DW_TAG_subrange_type) */
	.4byte	0x65	/* DW_AT_type */
	.byte	0x1	/* DW_AT_upper_bound */
	.byte	0	/* end of children of DIE 0x73 */
	.uleb128 0x9	/* (DIE (0x83) DW_TAG_variable) */
	.ascii "f\0"	/* DW_AT_name */
	.byte	0x1	/* DW_AT_decl_file (gdb.base/longest-types-64bit.c) */
	.byte	0x16	/* DW_AT_decl_line */
	.4byte	0x97	/* DW_AT_type */
	.byte	0x1	/* DW_AT_external */
	.byte	0x9	/* DW_AT_location */
	.byte	0x3	/* DW_OP_addr */
	.quad	f
	.uleb128 0xa	/* (DIE (0x97) DW_TAG_pointer_type) */
	.byte	0x8	/* DW_AT_byte_size */
	.4byte	0x1d	/* DW_AT_type */
	.byte	0	/* end of children of DIE 0xb */
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1	/* (abbrev code) */
	.uleb128 0x11	/* (TAG: DW_TAG_compile_unit) */
	.byte	0x1	/* DW_children_yes */
	.uleb128 0x25	/* (DW_AT_producer) */
	.uleb128 0xe	/* (DW_FORM_strp) */
	.uleb128 0x13	/* (DW_AT_language) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x3	/* (DW_AT_name) */
	.uleb128 0xe	/* (DW_FORM_strp) */
	.uleb128 0x1b	/* (DW_AT_comp_dir) */
	.uleb128 0xe	/* (DW_FORM_strp) */
	.uleb128 0x10	/* (DW_AT_stmt_list) */
	.uleb128 0x6	/* (DW_FORM_data4) */
	.byte	0
	.byte	0
	.uleb128 0x2	/* (abbrev code) */
	.uleb128 0x13	/* (TAG: DW_TAG_structure_type) */
	.byte	0x1	/* DW_children_yes */
	.uleb128 0x3	/* (DW_AT_name) */
	.uleb128 0x8	/* (DW_FORM_string) */
	.uleb128 0xb	/* (DW_AT_byte_size) */
	.uleb128 0x6	/* (DW_FORM_data4) */
	.uleb128 0x3a	/* (DW_AT_decl_file) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x3b	/* (DW_AT_decl_line) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x1	/* (DW_AT_sibling) */
	.uleb128 0x13	/* (DW_FORM_ref4) */
	.byte	0
	.byte	0
	.uleb128 0x3	/* (abbrev code) */
	.uleb128 0xd	/* (TAG: DW_TAG_member) */
	.byte	0	/* DW_children_no */
	.uleb128 0x3	/* (DW_AT_name) */
	.uleb128 0x8	/* (DW_FORM_string) */
	.uleb128 0x3a	/* (DW_AT_decl_file) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x3b	/* (DW_AT_decl_line) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x49	/* (DW_AT_type) */
	.uleb128 0x13	/* (DW_FORM_ref4) */
	.uleb128 0x38	/* (DW_AT_data_member_location) */
	.uleb128 0xa	/* (DW_FORM_block1) */
	.byte	0
	.byte	0
	.uleb128 0x4	/* (abbrev code) */
	.uleb128 0xd	/* (TAG: DW_TAG_member) */
	.byte	0	/* DW_children_no */
	.uleb128 0x3	/* (DW_AT_name) */
	.uleb128 0xe	/* (DW_FORM_strp) */
	.uleb128 0x3a	/* (DW_AT_decl_file) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x3b	/* (DW_AT_decl_line) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x49	/* (DW_AT_type) */
	.uleb128 0x13	/* (DW_FORM_ref4) */
	.uleb128 0x38	/* (DW_AT_data_member_location) */
	.uleb128 0xa	/* (DW_FORM_block1) */
	.byte	0
	.byte	0
	.uleb128 0x5	/* (abbrev code) */
	.uleb128 0x1	/* (TAG: DW_TAG_array_type) */
	.byte	0x1	/* DW_children_yes */
	.uleb128 0x49	/* (DW_AT_type) */
	.uleb128 0x13	/* (DW_FORM_ref4) */
	.uleb128 0x1	/* (DW_AT_sibling) */
	.uleb128 0x13	/* (DW_FORM_ref4) */
	.byte	0
	.byte	0
	.uleb128 0x6	/* (abbrev code) */
	.uleb128 0x21	/* (TAG: DW_TAG_subrange_type) */
	.byte	0	/* DW_children_no */
	.uleb128 0x49	/* (DW_AT_type) */
	.uleb128 0x13	/* (DW_FORM_ref4) */
	.uleb128 0x2f	/* (DW_AT_upper_bound) */
	.uleb128 0x7	/* (DW_FORM_data8) */
	.byte	0
	.byte	0
	.uleb128 0x7	/* (abbrev code) */
	.uleb128 0x24	/* (TAG: DW_TAG_base_type) */
	.byte	0	/* DW_children_no */
	.uleb128 0xb	/* (DW_AT_byte_size) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x3e	/* (DW_AT_encoding) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x3	/* (DW_AT_name) */
	.uleb128 0xe	/* (DW_FORM_strp) */
	.byte	0
	.byte	0
	.uleb128 0x8	/* (abbrev code) */
	.uleb128 0x21	/* (TAG: DW_TAG_subrange_type) */
	.byte	0	/* DW_children_no */
	.uleb128 0x49	/* (DW_AT_type) */
	.uleb128 0x13	/* (DW_FORM_ref4) */
	.uleb128 0x2f	/* (DW_AT_upper_bound) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.byte	0
	.byte	0
	.uleb128 0x9	/* (abbrev code) */
	.uleb128 0x34	/* (TAG: DW_TAG_variable) */
	.byte	0	/* DW_children_no */
	.uleb128 0x3	/* (DW_AT_name) */
	.uleb128 0x8	/* (DW_FORM_string) */
	.uleb128 0x3a	/* (DW_AT_decl_file) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x3b	/* (DW_AT_decl_line) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x49	/* (DW_AT_type) */
	.uleb128 0x13	/* (DW_FORM_ref4) */
	.uleb128 0x3f	/* (DW_AT_external) */
	.uleb128 0xc	/* (DW_FORM_flag) */
	.uleb128 0x2	/* (DW_AT_location) */
	.uleb128 0xa	/* (DW_FORM_block1) */
	.byte	0
	.byte	0
	.uleb128 0xa	/* (abbrev code) */
	.uleb128 0xf	/* (TAG: DW_TAG_pointer_type) */
	.byte	0	/* DW_children_no */
	.uleb128 0xb	/* (DW_AT_byte_size) */
	.uleb128 0xb	/* (DW_FORM_data1) */
	.uleb128 0x49	/* (DW_AT_type) */
	.uleb128 0x13	/* (DW_FORM_ref4) */
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.4byte	0x1c	/* Length of Address Ranges Info */
	.2byte	0x2	/* DWARF Version */
	.4byte	.Ldebug_info0	/* Offset of Compilation Unit Info */
	.byte	0x8	/* Size of Address */
	.byte	0	/* Size of Segment Descriptor */
	.2byte	0	/* Pad to 16 byte boundary */
	.2byte	0
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF4:
	.string	"gdb.base/longest-types-64bit.c"
.LASF3:
	.string	"GNU C 4.7.3 20121109 (prerelease)"
.LASF0:
	.string	"buf2"
.LASF1:
	.string	"sizetype"
.LASF5:
	.string	""
.LASF2:
	.string	"char"
	.ident	"GCC: (GNU) 4.7.3 20121109 (prerelease)"
	.section	.note.GNU-stack,"",@progbits
