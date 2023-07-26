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
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>


static void *
threader (void *arg)
{
	return NULL;
}

int
main (void)
{
	pthread_t t1;
	int i;

	i = pthread_create (&t1, NULL, threader, (void *) NULL);
	assert (i == 0);
	i = pthread_join (t1, NULL);
	assert (i == 0);

	execl ("/bin/true", "/bin/true", NULL);
	abort ();
}
