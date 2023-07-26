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

#if 0

void
foo (int size)
{
  typedef char array_t[size];
  array_t array;
  int i;

  for (i = 0; i < size; i++)
    array[i] = i;

  array[0] = 0;	/* break-here */
}

#else

void foo (int size);

int
main (void)
{
  foo (26);
  foo (78);
  return 0;
}

#endif
