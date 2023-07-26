/* Copyright 2007, 2009 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/*
 * Test GDB's handling of gcore for mapping with a name but zero inode.
 */

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

/* The same test running in a parallel testsuite may steal us the zero SID,
   even if we never get any EEXIST.  Just try a while.  */

#define TIMEOUT_SEC 10

static volatile int v;

static void
initialized (void)
{
  v++;
}

static void
unresolved (void)
{
  v++;
}

int
main (void)
{
  int sid;
  unsigned int *addr = (void *) -1L;
  int attempt, round = 0;
  time_t ts_start, ts;

  if (time (&ts_start) == (time_t) -1)
    {
      printf ("time (): %m\n");
      exit (1);
    }

  /* The generated SID will cycle with an increment of 32768, attempt until it
   * wraps to 0.  */

  for (attempt = 0; addr == (void *) -1L; attempt++)
    {
      /* kernel-2.6.25-8.fc9.x86_64 just never returns the value 0 by
	 shmget(2).  shmget returns SID range 0..1<<31 in steps of 32768,
	 0x1000 should be enough but wrap the range it to be sure.  */

      if (attempt > 0x21000)
        {
	  if (time (&ts) == (time_t) -1)
	    {
	      printf ("time (): %m\n");
	      exit (1);
	    }

	  if (ts >= ts_start && ts < ts_start + TIMEOUT_SEC)
	    {
	      attempt = 0;
	      round++;
	      continue;
	    }

	  printf ("Problem is not reproducible on this kernel (attempt %d, "
		  "round %d)\n", attempt, round);
	  unresolved ();
	  exit (1);
	}

      sid = shmget ((key_t) rand (), 0x1000, IPC_CREAT | IPC_EXCL | 0777);
      if (sid == -1)
	{
	  if (errno == EEXIST)
	    continue;

	  printf ("shmget (%d, 0x1000, IPC_CREAT): errno %d\n", 0, errno);
	  exit (1);
	}

      /* Use SID only if it is 0, retry it otherwise.  */

      if (sid == 0)
	{
	  addr = shmat (sid, NULL, SHM_RND);
	  if (addr == (void *) -1L)
	    {
	      printf ("shmat (%d, NULL, SHM_RND): errno %d\n", sid,
		      errno);
	      exit (1);
	    }
	}
      if (shmctl (sid, IPC_RMID, NULL) != 0)
	{
	  printf ("shmctl (%d, IPC_RMID, NULL): errno %d\n", sid, errno);
	  exit (1);
	}
    }

  initialized ();

  return 0;
}
