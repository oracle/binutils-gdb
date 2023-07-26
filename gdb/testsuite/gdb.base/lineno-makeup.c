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

/* DW_AT_low_pc-DW_AT_high_pc should cover the function without line number
   information (.debug_line) so we cannot use an external object file.
   
   It must not be just a label as it would alias on the next function even for
   correct GDB.  Therefore some stub data must be placed there.
   
   We need to provide a real stub function body as at least s390
   (s390_analyze_prologue) would skip the whole body till reaching `main'.  */

extern void func (void);
asm ("func: .incbin \"" BINFILENAME "\"");

int
main (void)
{
  func ();
  return 0;
}
