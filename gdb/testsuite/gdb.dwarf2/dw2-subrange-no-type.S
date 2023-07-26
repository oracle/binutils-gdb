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

/* Debug information */

	.section	.data
vardata:
	.rept		129
	.ascii		"x"
	.endr
	.ascii		"UNSEEN\0"

	.section	.debug_info
.Lcu1_begin:
	.4byte		.Lcu1_end - .Lcu1_start	/* Length of Compilation Unit */
.Lcu1_start:
	.2byte		2			/* DWARF version number */
	.4byte		.Ldebug_abbrev0		/* Offset Into Abbrev. Section */
	.byte		4			/* Pointer Size (in bytes) */

	/* CU die */
	.uleb128	1			/* Abbrev: DW_TAG_compile_unit */
	.ascii		"GNU C 3.3.3\0"		/* DW_AT_producer */
	.byte		2			/* DW_AT_language (C) -  */

.Larray_type:
	.uleb128	2			/* Abbrev: DW_TAG_array_type */
	.4byte		.Lchar_type-.Lcu1_begin	/* DW_AT_type */

	.uleb128	8			/* Abbrev: DW_TAG_subrange_type without DW_AT_type */
	.byte		0			/* DW_AT_lower_bound */
	.byte		128			/* DW_AT_upper_bound */

	.byte		0			/* End of children of die */

.Lchar_type:
	.uleb128	4			/* Abbrev: DW_TAG_base_type */
	.ascii		"char\0"		/* DW_AT_name */
	.byte		1			/* DW_AT_byte_size */
	.byte		6			/* DW_AT_encoding */

	.uleb128	6			/* Abbrev: DW_TAG_variable DW_FORM_string */
	.ascii		"notype_string\0"	/* DW_AT_name */
	.4byte		.Larray_type-.Lcu1_begin /* DW_AT_type */
	.byte		2f - 1f			/* DW_AT_location */
1:	.byte		3			/*   DW_OP_addr */
	.4byte		vardata			/*   <addr> */
2:

	.byte		0			/* End of children of CU */
.Lcu1_end:

	.section .debug_abbrev
.Ldebug_abbrev0:
	.uleb128	1			/* Abbrev code */
	.uleb128	0x11			/* DW_TAG_compile_unit */
	.byte		0x1			/* has_children */
	.uleb128	0x25			/* DW_AT_producer */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x13			/* DW_AT_language */
	.uleb128	0xb			/* DW_FORM_data1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	2			/* Abbrev code */
	.uleb128	0x1			/* TAG: DW_TAG_array_type */
	.byte		0x1			/* DW_children_yes */
	.uleb128	0x49			/* DW_AT_type */
	.uleb128	0x13			/* DW_FORM_ref4 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	4			/* Abbrev code */
	.uleb128	0x24			/* DW_TAG_base_type */
	.byte		0x0			/* no_children */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0xb			/* DW_AT_byte_size */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x3e			/* DW_AT_encoding */
	.uleb128	0xb			/* DW_FORM_data1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	6			/* Abbrev code */
	.uleb128	0x34			/* DW_TAG_variable */
	.byte		0x0			/* no_children */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x49			/* DW_AT_type */
	.uleb128	0x13			/* DW_FORM_ref4 */
	.uleb128	0x2			/* DW_AT_location */
	.uleb128	0xa			/* DW_FORM_block1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	8			/* Abbrev code */
	.uleb128	0x21			/* DW_TAG_subrange_type without DW_AT_type */
	.byte		0x0			/* no children */
	.uleb128	0x22			/* DW_AT_lower_bound */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x2f			/* DW_AT_upper_bound */
	.uleb128	0xb			/* DW_FORM_data1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.byte		0x0			/* Terminator */
