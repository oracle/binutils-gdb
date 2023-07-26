/* This testcase is part of GDB, the GNU debugger.

   Copyright 2016 Free Software Foundation, Inc.

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

#include <pthread.h>
#include <assert.h>

static void *
start (void *arg)
{
  return arg; /* Backtrace end breakpoint */
}

int
main (void)
{
  pthread_t thread1;
  int i;

  i = pthread_create (&thread1, NULL, start, NULL);
  assert (i == 0);
  i = pthread_join (thread1, NULL);
  assert (i == 0);

  return 0;
}
