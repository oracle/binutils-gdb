#include "b146835.h"
#include <iostream>

class F : public C {

protected:

   virtual void funcA (unsigned long a, B *b);
   virtual void funcB (E *e);
   virtual void funcC (unsigned long x, bool y);

   char *s1, *s2;
   bool b1;
   int k;

public:
   void foo() {
       std::cout << "foo" << std::endl;
   }
};


void F::funcA (unsigned long a, B *b) {}
void F::funcB (E *e) {}
void F::funcC (unsigned long x, bool y) {}

int  main()
{
   F f;
   f.foo();
}

