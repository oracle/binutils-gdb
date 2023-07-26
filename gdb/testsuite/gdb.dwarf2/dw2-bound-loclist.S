/* This testcase is part of GDB, the GNU debugger.

   Copyright 2010 Free Software Foundation, Inc.

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

/* We will `break *main' at the very first instruction.  */
#define main_length 1

	.section	.data
vardata:
	/* See DW_OP_lit3 + 1 (0-based).  */
	.string		"seennotseen"

	.section	.debug_info
.Lcu1_begin:
	.4byte		.Lcu1_end - .Lcu1_start	/* Length of Compilation Unit */
.Lcu1_start:
	.2byte		2			/* DWARF version number */
	.4byte		.Ldebug_abbrev0		/* Offset Into Abbrev. Section */
	.byte		4			/* Pointer Size (in bytes) */

	/* CU die */
	.uleb128	1			/* Abbrev: DW_TAG_compile_unit */
	.4byte		.Lproducer		/* DW_AT_producer */
	/* Use C++ to exploit a bug in parsing DW_AT_name "".  */
	.byte		4			/* DW_AT_language (C++) -  */
	.4byte		main			/* DW_AT_low_pc */
	.byte		main_length		/* DW_AT_high_pc */

.Larray_type:
	.uleb128	2			/* Abbrev: DW_TAG_array_type */
	.4byte		.Lchar_type-.Lcu1_begin	/* DW_AT_type */

	.uleb128	3			/* Abbrev: DW_TAG_subrange_type */
	.4byte		.Luint_type-.Lcu1_begin	/* DW_AT_type */
	.byte		0			/* DW_AT_lower_bound */
	.4byte		.Llen_var-.Lcu1_begin	/* DW_AT_upper_bound */
	.byte		0			/* End of children of die */

	/* DW_AT_upper_bound is referencing an optimized-out variable.  */
.Larrayb_type:
	.uleb128	2			/* Abbrev: DW_TAG_array_type */
	.4byte		.Lchar_type-.Lcu1_begin	/* DW_AT_type */

	.uleb128	3			/* Abbrev: DW_TAG_subrange_type */
	.4byte		.Luint_type-.Lcu1_begin	/* DW_AT_type */
	.byte		0			/* DW_AT_lower_bound */
	.4byte		.Llenb_var-.Lcu1_begin	/* DW_AT_upper_bound */
	.byte		0			/* End of children of die */

	/* DW_AT_upper_bound is referencing register.  */
.Larrayreg_type:
	.uleb128	2			/* Abbrev: DW_TAG_array_type */
	.4byte		.Lchar_type-.Lcu1_begin	/* DW_AT_type */

	.uleb128	8			/* Abbrev: DW_TAG_subrange_type with block */
	.4byte		.Luint_type-.Lcu1_begin	/* DW_AT_type */
	.byte		0			/* DW_AT_lower_bound */
	.byte		2f - 1f			/* DW_AT_upper_bound */
1:	.byte		0x50			/* DW_OP_reg0 */
2:
	.byte		0			/* End of children of die */

.Luint_type:
	.uleb128	4			/* Abbrev: DW_TAG_base_type */
	.4byte		.Luint_str		/* DW_AT_name */
	.byte		4			/* DW_AT_byte_size */
	.byte		7			/* DW_AT_encoding */

.Lchar_type:
	.uleb128	4			/* Abbrev: DW_TAG_base_type */
	.4byte		.Lchar_str		/* DW_AT_name */
	.byte		1			/* DW_AT_byte_size */
	.byte		6			/* DW_AT_encoding */

.Llen_var:
	.uleb128	5			/* Abbrev: DW_TAG_variable artificial */
	.byte		1			/* DW_AT_artificial */
	.4byte		.Luint_type-.Lcu1_begin	/* DW_AT_type */
	.4byte		.Llen_loclist-.Lloclist	/* DW_AT_location */

	/* optimized-out variable for b_string.  */
.Llenb_var:
	.uleb128	7			/* Abbrev: DW_TAG_variable artificial no DW_AT_location */
	.byte		1			/* DW_AT_artificial */
	.4byte		.Luint_type-.Lcu1_begin	/* DW_AT_type */

	.uleb128	6			/* Abbrev: DW_TAG_variable DW_FORM_string */
	.string		"a_string"		/* DW_AT_name */
	.4byte		.Larray_type-.Lcu1_begin /* DW_AT_type */
	.byte		2f - 1f			/* DW_AT_location */
1:	.byte		3			/*   DW_OP_addr */
	.4byte		vardata			/*   <addr> */
2:

	/* DW_AT_upper_bound is referencing an optimized-out variable.  */
	.uleb128	6			/* Abbrev: DW_TAG_variable DW_FORM_string */
	.string		"b_string"		/* DW_AT_name */
	.4byte		.Larrayb_type-.Lcu1_begin /* DW_AT_type */
	.byte		2f - 1f			/* DW_AT_location */
1:	.byte		3			/*   DW_OP_addr */
	.4byte		vardata			/*   <addr> */
2:

	/* DW_AT_upper_bound is referencing register.  */
	.uleb128	6			/* Abbrev: DW_TAG_variable DW_FORM_string */
	.string		"reg_string"		/* DW_AT_name */
	.4byte		.Larrayreg_type-.Lcu1_begin /* DW_AT_type */
	.byte		2f - 1f			/* DW_AT_location */
1:	.byte		3			/*   DW_OP_addr */
	.4byte		vardata			/*   <addr> */
2:

	.byte		0			/* End of children of CU */
.Lcu1_end:

	.section	.debug_loc
.Lloclist:
.Llen_loclist:
	.4byte	0			# Location list begin address
	.4byte	main_length		# Location list end address
	.value	2f-1f	# Location expression size
1:	.byte	0x33	# DW_OP_lit3
	.byte	0x9f	# DW_OP_stack_value
2:
	.quad	0x0	# Location list terminator begin (*.LLST2)
	.quad	0x0	# Location list terminator end (*.LLST2)

	.section .debug_abbrev
.Ldebug_abbrev0:
	.uleb128	1			/* Abbrev code */
	.uleb128	0x11			/* DW_TAG_compile_unit */
	.byte		0x1			/* has_children */
	.uleb128	0x25			/* DW_AT_producer */
	.uleb128	0xe			/* DW_FORM_strp */
	.uleb128	0x13			/* DW_AT_language */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x11			/* DW_AT_low_pc */
	.uleb128	0x1			/* DW_FORM_addr */
	.uleb128	0x12			/* DW_AT_high_pc */
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

	.uleb128	3			/* Abbrev code */
	.uleb128	0x21			/* DW_TAG_subrange_type */
	.byte		0x0			/* no children */
	.uleb128	0x49			/* DW_AT_type */
	.uleb128	0x13			/* DW_FORM_ref4 */
	.uleb128	0x22			/* DW_AT_lower_bound */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x2f			/* DW_AT_upper_bound */
	.uleb128	0x13			/* DW_FORM_ref4 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	4			/* Abbrev code */
	.uleb128	0x24			/* DW_TAG_base_type */
	.byte		0x0			/* no_children */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0xe			/* DW_FORM_strp */
	.uleb128	0xb			/* DW_AT_byte_size */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x3e			/* DW_AT_encoding */
	.uleb128	0xb			/* DW_FORM_data1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	5			/* Abbrev code */
	.uleb128	0x34			/* DW_TAG_variable */
	.byte		0x0			/* no_children */
	.uleb128	0x34			/* DW_AT_artificial */
	.uleb128	0x0c			/* DW_FORM_flag */
	.uleb128	0x49			/* DW_AT_type */
	.uleb128	0x13			/* DW_FORM_ref4 */
	.uleb128	0x02			/* DW_AT_location */
	.uleb128	0x06			/* DW_FORM_data4 */
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

	.uleb128	7			/* Abbrev code */
	.uleb128	0x34			/* DW_TAG_variable */
	.byte		0x0			/* no_children */
	.uleb128	0x34			/* DW_AT_artificial */
	.uleb128	0x0c			/* DW_FORM_flag */
	.uleb128	0x49			/* DW_AT_type */
	.uleb128	0x13			/* DW_FORM_ref4 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	8			/* Abbrev code */
	.uleb128	0x21			/* DW_TAG_subrange_type with block */
	.byte		0x0			/* no children */
	.uleb128	0x49			/* DW_AT_type */
	.uleb128	0x13			/* DW_FORM_ref4 */
	.uleb128	0x22			/* DW_AT_lower_bound */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x2f			/* DW_AT_upper_bound */
	.uleb128	0xa			/* DW_FORM_block1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.byte		0x0			/* Terminator */

/* String table */
	.section .debug_str
.Lproducer:
	.string		"GNU C 3.3.3"
.Lchar_str:
	.string		"char"
.Luint_str:
	.string		"unsigned int"
