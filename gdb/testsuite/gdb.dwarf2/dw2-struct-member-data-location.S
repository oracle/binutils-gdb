/* This testcase is part of GDB, the GNU debugger.

   Copyright 2009 Free Software Foundation, Inc.

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
	.ascii	"dw2-struct-member-data-location.c\0"	/* DW_AT_name */
	.ascii	"GNU C 4.3.2\0"			/* DW_AT_producer */
	.byte	1				/* DW_AT_language (C) */

.Ltype_uchar:
	.uleb128	2			/* Abbrev: DW_TAG_structure_type */
	.ascii		"some_struct\0"		/* DW_AT_name */

	.uleb128	3			/* Abbrev: DW_TAG_member */
	.ascii		"field\0"		/* DW_AT_name */
	.byte		0			/* DW_AT_data_member_location */

	.byte		0			/* End of children of some_struct */

	.byte		0			/* End of children of CU */

.Lcu1_end:

/* Abbrev table */
	.section .debug_abbrev
.Labbrev1_begin:
	.uleb128	1			/* Abbrev code */
	.uleb128	0x11			/* DW_TAG_compile_unit */
	.byte		1			/* has_children */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x25			/* DW_AT_producer */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x13			/* DW_AT_language */
	.uleb128	0xb			/* DW_FORM_data1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	2			/* Abbrev code */
	.uleb128	0x13			/* DW_TAG_structure_type */
	.byte		1			/* has_children */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0x8			/* DW_FORM_string */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	3			/* Abbrev code */
	.uleb128	0x0d			/* DW_TAG_member */
	.byte		0			/* has_children */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x38			/* DW_AT_data_member_location */
	.uleb128	0x0b			/* DW_FORM_data1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */
