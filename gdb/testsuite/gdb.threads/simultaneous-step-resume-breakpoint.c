/* Copyright 2009 Free Software Foundation, Inc.

   Written by Fred Fish of Cygnus Support
   Contributed by Cygnus Support

   This file is part of GDB.

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

/* Test multiple threads stepping into a .debug_line-less function with
   a breakpoint placed on its return-to-caller point.  */

#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#define THREADS 3

static void *
func (void *unused)
{
  int i;

  errno = 0;
  i = 0xdeadf00d;
  i = sleep (THREADS);	/* sleep-call */
  if (errno != 0)	/* sleep-after */
    perror ("sleep");

  /* The GDB bug with forgotten step-resume breakpoint could leave stale
     breakpoint on the I assignment making it a nop.  */
  if (i == 0xdeadf00d)
    assert (0);

  assert (i == 0);

  pthread_exit (NULL);
}

int
main (void)
{
  pthread_t threads[THREADS];
  int threadi;

  for (threadi = 0; threadi < THREADS; threadi++)
    {
      int i;

      i = pthread_create (&threads[threadi], NULL, func, NULL);
      assert (i == 0);

      i = sleep (1);
      assert (i == 0);
    }

  for (threadi = 0; threadi < THREADS; threadi++)
    {
      int i;

      i = pthread_join (threads[threadi], NULL);
      assert (i == 0);
    }

  return 0;	/* final-exit */
}
