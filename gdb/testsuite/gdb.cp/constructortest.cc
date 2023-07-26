/* This testcase is part of GDB, the GNU debugger.

   Copyright 2005 Free Software Foundation, Inc.

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

class A
{
  public:
    A();
    ~A();
    int  k;
  private:
    int  x;
};

class B: public A
{
  public:
    B();
  private:
    int  y;
};

/* C and D are for the $delete destructor.  */

class C
{
  public:
    C();
    virtual ~C();
  private:
    int  x;
};

class D: public C
{
  public:
    D();
  private:
    int  y;
};

int main(int argc, char *argv[])
{
  A* a = new A;
  B* b = new B;
  D* d = new D;
  delete a;
  delete b;
  delete d;
  return 0;
}

A::A() /* Constructor A */
{
   x = 1; /* First line A */
   k = 4; /* Second line A */
}

A::~A() /* Destructor A */
{
   x = 3; /* First line ~A */
   k = 6; /* Second line ~A */
}

B::B()
{
   y = 2; /* First line B */
   k = 5;
}

C::C() /* Constructor C */
{
   x = 1; /* First line C */
}

C::~C() /* Destructor C */
{
   x = 3; /* First line ~C */
}

D::D()
{
   y = 2; /* First line D */
}
