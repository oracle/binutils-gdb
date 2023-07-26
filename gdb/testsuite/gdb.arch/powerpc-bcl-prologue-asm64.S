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

	.section	".toc","aw"
	.section	".text"
	.align 2
	.globl func0
	.section	".opd","aw"
	.align 3
func0:
	.quad	.L.func0,.TOC.@tocbase
	.previous
	.type	func0, @function
.L.func0:
	mflr 0
	std 31,-8(1)
	std 0,16(1)
	stdu 1,-128(1)
	mr 31,1
	bl abort
	nop
	.long 0
	.byte 0,0,0,1,128,1,0,1
	.size	func0,.-.L.func0
	.section	".toc","aw"
.LC1:
	.tc var[TC],var
	.section	".text"
	.align 2
	.globl func1
	.section	".opd","aw"
	.align 3
func1:
	.quad	.L.func1,.TOC.@tocbase
	.previous
	.type	func1, @function
.L.func1:
	mflr 0
/* 20 = BO = branch always
   31 = BI = CR bit (ignored)  */
	bcl 20,31,.Lpie
.Lpie:	std 31,-8(1)
	std 0,16(1)
	stdu 1,-128(1)
	mr 31,1
	bl func0
	mr 11,3
	ld 9,.LC1@toc(2)
	lwz 0,0(9)
	add 0,11,0
	extsw 0,0
	mr 3,0
	ld 1,0(1)
	ld 0,16(1)
	mtlr 0
	ld 31,-8(1)
	blr
	.long 0
	.byte 0,0,0,1,128,1,0,1
	.size	func1,.-.L.func1
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
