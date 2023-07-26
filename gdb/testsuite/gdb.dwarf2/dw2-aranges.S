/* This testcase is part of GDB, the GNU debugger.

   Copyright 2004, 2007, 2008, 2009 Free Software Foundation, Inc.

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

/* Test .debug_aranges containing zero address_size.  */

/* Dummy function to provide debug information for.  */

	.text
.Lbegin_text1:
	.globl main
	.type main, %function
main:
.Lbegin_main:
	.int 0
.Lend_main:
	.size main, .-main
.Lend_text1:

/* Debug information */

	.section .debug_info
.Lcu1_begin:
	/* CU header */
	.4byte	.Lcu1_end - .Lcu1_start		/* Length of Compilation Unit */
.Lcu1_start:
	.2byte	2				/* DWARF Version */
	.4byte	.Labbrev1_begin			/* Offset into abbrev section */
	.byte	4				/* Pointer size */

	/* CU die */
	.uleb128 1				/* Abbrev: DW_TAG_compile_unit */
	.4byte	.Lend_text1			/* DW_AT_high_pc */
	.4byte	.Lbegin_text1			/* DW_AT_low_pc */
	.ascii	"file1.txt\0"			/* DW_AT_name */
	.ascii	"GNU C 3.3.3\0"			/* DW_AT_producer */
	.byte	1				/* DW_AT_language (C) */

	/* main */
	.uleb128	2			/* Abbrev: DW_TAG_subprogram */
	.byte		1			/* DW_AT_external */
	.byte		1			/* DW_AT_decl_file */
	.byte		2			/* DW_AT_decl_line */
	.ascii		"main\0"		/* DW_AT_name */
	.4byte		.Ltype_int-.Lcu1_begin	/* DW_AT_type */
	.4byte		.Lbegin_main	/* DW_AT_low_pc */
	.4byte		.Lend_main		/* DW_AT_high_pc */
	.byte		1			/* DW_AT_frame_base: length */
	.byte		0x55			/* DW_AT_frame_base: DW_OP_reg5 */

.Ltype_int:
	.uleb128	3			/* Abbrev: DW_TAG_base_type */
	.ascii		"int\0"			/* DW_AT_name */
	.byte		4			/* DW_AT_byte_size */
	.byte		5			/* DW_AT_encoding */

	.byte		0			/* End of children of CU */

.Lcu1_end:

/* Abbrev table */
	.section .debug_abbrev
.Labbrev1_begin:
	.uleb128	1			/* Abbrev code */
	.uleb128	0x11			/* DW_TAG_compile_unit */
	.byte		1			/* has_children */
	.uleb128	0x12			/* DW_AT_high_pc */
	.uleb128	0x1			/* DW_FORM_addr */
	.uleb128	0x11			/* DW_AT_low_pc */
	.uleb128	0x1			/* DW_FORM_addr */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x25			/* DW_AT_producer */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x13			/* DW_AT_language */
	.uleb128	0xb			/* DW_FORM_data1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	2			/* Abbrev code */
	.uleb128	0x2e			/* DW_TAG_subprogram */
	.byte		0			/* has_children */
	.uleb128	0x3f			/* DW_AT_external */
	.uleb128	0xc			/* DW_FORM_flag */
	.uleb128	0x3a			/* DW_AT_decl_file */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x3b			/* DW_AT_decl_line */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x49			/* DW_AT_type */
	.uleb128	0x13			/* DW_FORM_ref4 */
	.uleb128	0x11			/* DW_AT_low_pc */
	.uleb128	0x1			/* DW_FORM_addr */
	.uleb128	0x12			/* DW_AT_high_pc */
	.uleb128	0x1			/* DW_FORM_addr */
	.uleb128	0x40			/* DW_AT_frame_base */
	.uleb128	0xa			/* DW_FORM_block1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	3			/* Abbrev code */
	.uleb128	0x24			/* DW_TAG_base_type */
	.byte		0			/* has_children */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0xb			/* DW_AT_byte_size */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x3e			/* DW_AT_encoding */
	.uleb128	0xb			/* DW_FORM_data1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

/* aranges table */
	.section .debug_aranges
	.long	.Laranges_end - 1f
1:
	.2byte	2				/* aranges Version */
	.4byte	.Lcu1_begin - .debug_info	/* Offset into .debug_info section */
	/* The GDB crasher is this zero value.  */
	.byte		0			/* aranges address_size */
	.byte		0			/* aranges segment_size */

.Laranges_end:
