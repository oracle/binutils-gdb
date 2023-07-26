/* Testcase for recursive dlopen calls.

   Copyright (C) 2014 Free Software Foundation, Inc.

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

/* This test was copied from glibc's testcase called
   <dlfcn/tst-rec-dlopen.c> and related files.  */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <dlfcn.h>

#define DSO "gdb-rhbz1156192-recursive-dlopen-libfoo.so"
#define FUNC "foo"

#define DSO1 "gdb-rhbz1156192-recursive-dlopen-libbar.so"
#define FUNC1 "bar"

/* Prototype for my hook.  */
void *custom_malloc_hook (size_t, const void *);

/* Pointer to old malloc hooks.  */
void *(*old_malloc_hook) (size_t, const void *);

/* Call function func_name in DSO dso_name via dlopen.  */
void
call_func (const char *dso_name, const char *func_name)
{
  int ret;
  void *dso;
  void (*func) (void);
  char *err;

  /* Open the DSO.  */
  dso = dlopen (dso_name, RTLD_NOW|RTLD_GLOBAL);
  if (dso == NULL)
    {
      err = dlerror ();
      fprintf (stderr, "%s\n", err);
      exit (1);
    }
  /* Clear any errors.  */
  dlerror ();

  /* Lookup func.  */
  *(void **) (&func) = dlsym (dso, func_name);
  if (func == NULL)
    {
      err = dlerror ();
      if (err != NULL)
        {
  fprintf (stderr, "%s\n", err);
  exit (1);
        }
    }
  /* Call func twice.  */
  (*func) ();

  /* Close the library and look for errors too.  */
  ret = dlclose (dso);
  if (ret != 0)
    {
      err = dlerror ();
      fprintf (stderr, "%s\n", err);
      exit (1);
    }

}

/* Empty hook that does nothing.  */
void *
custom_malloc_hook (size_t size, const void *caller)
{
  void *result;
  /* Restore old hooks.  */
  __malloc_hook = old_malloc_hook;
  /* First call a function in another library via dlopen.  */
  call_func (DSO1, FUNC1);
  /* Called recursively.  */
  result = malloc (size);
  /* Restore new hooks.  */
  __malloc_hook = custom_malloc_hook;
  return result;
}

int
main (void)
{

  /* Save old hook.  */
  old_malloc_hook = __malloc_hook;
  /* Install new hook.  */
  __malloc_hook = custom_malloc_hook;

  /* Attempt to dlopen a shared library. This dlopen will
     trigger an access to the ld.so.cache, and that in turn
     will require a malloc to duplicate data in the cache.
     The malloc will call our malloc hook which calls dlopen
     recursively, and upon return of this dlopen the non-ref
     counted ld.so.cache mapping will be unmapped. We will
     return to the original dlopen and crash trying to access
     dlopened data.  */
  call_func (DSO, FUNC);

  /* Restore old hook.  */
  __malloc_hook = old_malloc_hook;

  return 0;
}
