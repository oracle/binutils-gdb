/* This testcase is part of GDB, the GNU debugger.

   Copyright 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

	.section	".text"
	.align 2
	.globl func0
	.type	func0, @function
func0:
	stwu 1,-16(1)
	mflr 0
	stw 31,12(1)
	stw 0,20(1)
	mr 31,1
	bl abort
	.size	func0, .-func0
	.align 2
	.globl func1
	.type	func1, @function
func1:
	stwu 1,-16(1)
	mflr 0
/* 20 = BO = branch always
   31 = BI = CR bit (ignored)  */
	bcl 20,31,.Lpie
.Lpie:	stw 31,12(1)
	stw 0,20(1)
	mr 31,1
	bl func0
	mr 0,3
	lis 9,var@ha
	lwz 9,var@l(9)
	add 0,0,9
	mr 3,0
	lwz 11,0(1)
	lwz 0,4(11)
	mtlr 0
	lwz 31,-4(11)
	mr 1,11
	blr
	.size	func1, .-func1
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.4.6 20060404 (Red Hat 3.4.6-8)"

/* Original source file:

#include <stdlib.h>

extern volatile int var;

int func0 (void) __attribute__((__noinline__));
int func0 (void)
{
  abort ();
  return var;
}

int func1 (void) __attribute__((__noinline__));
int func1 (void)
{
  return func0 () + var;
}

*/
