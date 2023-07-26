/* Copyright 2011 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   This file is part of the gdb testsuite.  */

	.globl _start
_start:	.text

/* gas/i386/rdrnd.s */
	.att_syntax prefix
	rdrand %bx
	rdrand %ebx

	.intel_syntax noprefix
	rdrand bx
	rdrand ebx

/* gas/i386/f16c.s */
	.att_syntax prefix
	vcvtph2ps %xmm4,%ymm4
	vcvtph2ps (%ecx),%ymm4
	vcvtph2ps %xmm4,%xmm6
	vcvtph2ps (%ecx),%xmm4
	vcvtps2ph $0x2,%ymm4,%xmm4
	vcvtps2ph $0x2,%ymm4,(%ecx) 
	vcvtps2ph $0x2,%xmm4,%xmm4
	vcvtps2ph $0x2,%xmm4,(%ecx) 

	.intel_syntax noprefix
	vcvtph2ps ymm4,xmm4
	vcvtph2ps ymm4,XMMWORD PTR [ecx]
	vcvtph2ps ymm4,[ecx]
	vcvtph2ps xmm6,xmm4
	vcvtph2ps xmm4,QWORD PTR [ecx]          
	vcvtph2ps xmm4,[ecx]
	vcvtps2ph xmm4,ymm4,0x2
	vcvtps2ph XMMWORD PTR [ecx],ymm4,0x2    
	vcvtps2ph [ecx],ymm4,0x2 
	vcvtps2ph xmm4,xmm4,0x2
	vcvtps2ph QWORD PTR [ecx],xmm4,0x2      
	vcvtps2ph [ecx],xmm4,0x2 

/* gas/i386/fsgs.s */
	.att_syntax prefix
	rdfsbase %ebx
	rdgsbase %ebx
	wrfsbase %ebx
	wrgsbase %ebx

	.intel_syntax noprefix
	rdfsbase ebx    
	rdgsbase ebx    
	wrfsbase ebx    
	wrgsbase ebx    

