/* Copyright (C) 2021 Free Software Foundation, Inc.
   Contributed by Oracle.

   This file is part of GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#ifndef _GP_DEFS_H_
#define _GP_DEFS_H_

/* Define the ARCH and WSIZE predicates */
/*
 * The way we define and use predicates is similar to the 
 * standard #assert with one important exception:
 * if an argument of a predicate is not known the result
 * is 'false' and we want a compile time error to avoid
 * silent results from typos like ARCH(INTEL), COMPILER(gnu),
 * etc.
 */
#define OS(x)               TOK_O_##x(OS)
#define TOK_O_Solaris(x)    x##_Solaris
#define TOK_O_Linux(x)      x##_Linux

#define ARCH(x)             TOK_A_##x(ARCH)
#define TOK_A_Aarch64(x)    x##_Aarch64
#define TOK_A_SPARC(x)      x##_SPARC
#define TOK_A_Intel(x)      x##_Intel

#define WSIZE(x)            TOK_W_##x(WSIZE)
#define TOK_W_32(x)         x##_32
#define TOK_W_64(x)         x##_64


#if defined(linux) || defined(__linux)
#define OS_Linux            1
#else
#define OS_Solaris          1
#endif

#if defined(sparc) || defined(__sparcv9)
#define ARCH_SPARC          1
#elif defined(__i386) || defined(__amd64)
#define ARCH_Intel          1
#elif defined(__aarch64)
#define ARCH_Aarch64        1
#else
#error "Undefined platform"
#endif

#if defined(__sparcv9) || defined(__amd64) || defined(__aarch64)
#define WSIZE_64            1
#else
#define WSIZE_32            1
#endif

#endif
