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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#ifdef THREADS

# include <pthread.h>

static void *
threader (void *arg)
{
  return NULL;
}

#endif

int
main (int argc, char **argv)
{
  char *exec_nothreads, *exec_threads, *cmd;
  int phase;
  char phase_s[8];

  setbuf (stdout, NULL);

  if (argc != 4)
    {
      fprintf (stderr, "%s <non-threaded> <threaded> <phase>\n", argv[0]);
      return 1;
    }

#ifdef THREADS
  puts ("THREADS: Y");
#else
  puts ("THREADS: N");
#endif
  exec_nothreads = argv[1];
  printf ("exec_nothreads: %s\n", exec_nothreads);
  exec_threads = argv[2];
  printf ("exec_threads: %s\n", exec_threads);
  phase = atoi (argv[3]);
  printf ("phase: %d\n", phase);

  /* Phases: threading
     0: N -> N
     1: N -> Y
     2: Y -> Y
     3: Y -> N
     4: N -> exit  */

  cmd = NULL;

#ifndef THREADS
  switch (phase)
    {
    case 0:
      cmd = exec_nothreads;
      break;
    case 1:
      cmd = exec_threads;
      break;
    case 2:
      fprintf (stderr, "%s: We should have threads for phase %d!\n", argv[0],
	       phase);
      return 1;
    case 3:
      fprintf (stderr, "%s: We should have threads for phase %d!\n", argv[0],
	       phase);
      return 1;
    case 4:
      return 0;
    default:
      assert (0);
    }
#else	/* THREADS */
  switch (phase)
    {
    case 0:
      fprintf (stderr, "%s: We should not have threads for phase %d!\n",
	       argv[0], phase);
      return 1;
    case 1:
      fprintf (stderr, "%s: We should not have threads for phase %d!\n",
	       argv[0], phase);
      return 1;
    case 2:
      cmd = exec_threads;
      {
	pthread_t t1;
	int i;

	i = pthread_create (&t1, NULL, threader, (void *) NULL);
	assert (i == 0);
	i = pthread_join (t1, NULL);
	assert (i == 0);
      }
      break;
    case 3:
      cmd = exec_nothreads;
      {
	pthread_t t1;
	int i;

	i = pthread_create (&t1, NULL, threader, (void *) NULL);
	assert (i == 0);
	i = pthread_join (t1, NULL);
	assert (i == 0);
      }
      break;
    case 4:
      fprintf (stderr, "%s: We should not have threads for phase %d!\n",
	       argv[0], phase);
      return 1;
    default:
      assert (0);
    }
#endif	/* THREADS */

  assert (cmd != NULL);

  phase++;
  snprintf (phase_s, sizeof phase_s, "%d", phase);

  execl (cmd, cmd, exec_nothreads, exec_threads, phase_s, NULL);
  assert (0);
}
