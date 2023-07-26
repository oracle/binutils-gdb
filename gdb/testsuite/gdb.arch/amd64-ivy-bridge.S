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

/* gas/i386/x86-64-rdrnd.s */
	.att_syntax prefix
	rdrand %bx
	rdrand %ebx
	rdrand %rbx
	rdrand %r8w
	rdrand %r8d
	rdrand %r8

	.intel_syntax noprefix
	rdrand bx
	rdrand ebx
	rdrand rbx
	rdrand r8w
	rdrand r8d
	rdrand r8

/* gas/i386/x86-64-f16c.s */
	.att_syntax prefix
	vcvtph2ps %xmm4,%ymm4
	vcvtph2ps (%r8),%ymm8
	vcvtph2ps %xmm4,%xmm6
	vcvtph2ps (%rcx),%xmm4
	vcvtps2ph $0x2,%ymm4,%xmm4
	vcvtps2ph $0x2,%ymm8,(%r8)
	vcvtps2ph $0x2,%xmm4,%xmm4
	vcvtps2ph $0x2,%xmm4,(%rcx) 

	.intel_syntax noprefix
	vcvtph2ps ymm4,xmm4
	vcvtph2ps ymm8,XMMWORD PTR [r8]
	vcvtph2ps ymm4,[rcx]
	vcvtph2ps xmm6,xmm4
	vcvtph2ps xmm4,QWORD PTR [rcx]          
	vcvtph2ps xmm4,[rcx]
	vcvtps2ph xmm4,ymm4,0x2
	vcvtps2ph XMMWORD PTR [rcx],ymm4,0x2    
	vcvtps2ph [rcx],ymm4,0x2 
	vcvtps2ph xmm4,xmm4,0x2
	vcvtps2ph QWORD PTR [r8],xmm8,0x2       
	vcvtps2ph [rcx],xmm4,0x2 

/* gas/i386/x86-64-fsgs.s */
	.att_syntax prefix
	rdfsbase %ebx
	rdfsbase %rbx
	rdfsbase %r8d
	rdfsbase %r8
	rdgsbase %ebx
	rdgsbase %rbx   
	rdgsbase %r8d   
	rdgsbase %r8    
	wrfsbase %ebx   
	wrfsbase %rbx   
	wrfsbase %r8d   
	wrfsbase %r8    
	wrgsbase %ebx   
	wrgsbase %rbx   
	wrgsbase %r8d   
	wrgsbase %r8    

	.intel_syntax noprefix
	rdfsbase ebx    
	rdfsbase rbx    
	rdfsbase r8d    
	rdfsbase r8
	rdgsbase ebx    
	rdgsbase rbx    
	rdgsbase r8d    
	rdgsbase r8
	wrfsbase ebx    
	wrfsbase rbx    
	wrfsbase r8d    
	wrfsbase r8
	wrgsbase ebx    
	wrgsbase rbx    
	wrgsbase r8d    
	wrgsbase r8
