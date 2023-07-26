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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* Test stepping over RISC atomic sequences.
   This variant testcases the code for stepping another thread while skipping
   over the atomic sequence in the former thread
   (STEPPING_PAST_SINGLESTEP_BREAKPOINT).
   Code comes from gcc/testsuite/gcc.dg/sync-2.c  */

/* { dg-options "-march=i486" { target { { i?86-*-* x86_64-*-* } && ilp32 } } } */
/* { dg-options "-mcpu=v9" { target sparc*-*-* } } */

/* Test functionality of the intrinsics for 'short' and 'char'.  */

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>

#define LOOPS 2

static int unused;

static char AI[18];
static char init_qi[18] = { 3,5,7,9,0,0,0,0,-1,0,0,0,0,0,-1,0,0,0 };
static char test_qi[18] = { 3,5,7,9,1,4,22,-12,7,8,9,7,1,-12,7,8,9,7 };

static void
do_qi (void)
{
  if (__sync_fetch_and_add(AI+4, 1) != 0)
    abort ();
  if (__sync_fetch_and_add(AI+5, 4) != 0)
    abort ();
  if (__sync_fetch_and_add(AI+6, 22) != 0)
    abort ();
  if (__sync_fetch_and_sub(AI+7, 12) != 0)
    abort ();
  if (__sync_fetch_and_and(AI+8, 7) != (char)-1)
    abort ();
  if (__sync_fetch_and_or(AI+9, 8) != 0)
    abort ();
  if (__sync_fetch_and_xor(AI+10, 9) != 0)
    abort ();
  if (__sync_fetch_and_nand(AI+11, 7) != 0)
    abort ();

  if (__sync_add_and_fetch(AI+12, 1) != 1)
    abort ();
  if (__sync_sub_and_fetch(AI+13, 12) != (char)-12)
    abort ();
  if (__sync_and_and_fetch(AI+14, 7) != 7)
    abort ();
  if (__sync_or_and_fetch(AI+15, 8) != 8)
    abort ();
  if (__sync_xor_and_fetch(AI+16, 9) != 9)
    abort ();
  if (__sync_nand_and_fetch(AI+17, 7) != 7)
    abort ();
}

static short AL[18];
static short init_hi[18] = { 3,5,7,9,0,0,0,0,-1,0,0,0,0,0,-1,0,0,0 };
static short test_hi[18] = { 3,5,7,9,1,4,22,-12,7,8,9,7,1,-12,7,8,9,7 };

static void
do_hi (void)
{
  if (__sync_fetch_and_add(AL+4, 1) != 0)
    abort ();
  if (__sync_fetch_and_add(AL+5, 4) != 0)
    abort ();
  if (__sync_fetch_and_add(AL+6, 22) != 0)
    abort ();
  if (__sync_fetch_and_sub(AL+7, 12) != 0)
    abort ();
  if (__sync_fetch_and_and(AL+8, 7) != -1)
    abort ();
  if (__sync_fetch_and_or(AL+9, 8) != 0)
    abort ();
  if (__sync_fetch_and_xor(AL+10, 9) != 0)
    abort ();
  if (__sync_fetch_and_nand(AL+11, 7) != 0)
    abort ();

  if (__sync_add_and_fetch(AL+12, 1) != 1)
    abort ();
  if (__sync_sub_and_fetch(AL+13, 12) != -12)
    abort ();
  if (__sync_and_and_fetch(AL+14, 7) != 7)
    abort ();
  if (__sync_or_and_fetch(AL+15, 8) != 8)
    abort ();
  if (__sync_xor_and_fetch(AL+16, 9) != 9)
    abort ();
  if (__sync_nand_and_fetch(AL+17, 7) != 7)
    abort ();
}

static void *
start1 (void *arg)
{
  unsigned loop;
  sleep(1);

  for (loop = 0; loop < LOOPS; loop++)
    {
      memcpy(AI, init_qi, sizeof(init_qi));

      do_qi ();

      if (memcmp (AI, test_qi, sizeof(test_qi)))
	abort ();
    }

  return arg;						/* _delete1_ */
}

static void *
start2 (void *arg)
{
  unsigned loop;

  for (loop = 0; loop < LOOPS; loop++)
    {
      memcpy(AL, init_hi, sizeof(init_hi));

      do_hi ();

      if (memcmp (AL, test_hi, sizeof(test_hi)))
	abort ();
    }

  return arg;						/* _delete2_ */
}

int
main (int argc, char **argv)
{
  pthread_t thread;
  int i;

  i = pthread_create (&thread, NULL, start1, NULL);	/* _create_ */
  assert (i == 0);					/* _create_after_ */

  sleep (1);

  start2 (NULL);

  i = pthread_join (thread, NULL);			/* _delete_ */
  assert (i == 0);

  return 0;						/* _exit_ */
}
