/* Copyright 2014-2020 Free Software Foundation, Inc.

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

/* This C file simulates the implementation of object pointers in
   GraalVM Java native images where the object data is not addressed
   directly. It serves as a regression test for a bug where printing
   of such redirected data structures suffers from a gdb exception.

   Debugging information on how to decode an object pointer to
   identify the address of the underlying data will be generated
   separately by the testcase using that file. */

#include <stdlib.h>
#include <string.h>

struct Object {
  struct Object *next;
  int val;
};

struct Object *testOop;

extern int debugMe() {
  return 0;
}

struct Object *newObject() {
  char *bytes = malloc(sizeof(struct Object));
  return (struct Object *)bytes;
}

int
main (void)
{
  struct Object *obj1 = newObject();
  struct Object *obj2 = newObject();
  struct Object *obj3 = newObject();
  obj1->val = 0;
  obj2->val = 1;
  obj3->val = 2;

  obj1->next = obj2;
  obj2->next = obj3;
  obj3->next = obj1;

  testOop = obj1;

  return debugMe();
}
