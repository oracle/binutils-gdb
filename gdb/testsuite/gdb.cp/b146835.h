
class A {

protected:

   virtual void funcA (unsigned long a, class B *b) = 0;
   virtual void funcB (class E *e) = 0;
   virtual void funcC (unsigned long x, bool y) = 0;

   void funcD (class E *e, class D* d);
   virtual void funcE (E *e, D *d);
   virtual void funcF (unsigned long x, D *d);
};


class C : public A {

protected:

   int x;
   class K *k;
   class H *h;

   D *d;

   class W *w;
   class N *n;
   class L *l;
   unsigned long *r;

public:

   C();
   int z (char *s);
   virtual ~C();
};
