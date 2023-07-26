#include <iostream>
#include <pthread.h>

class x
  {
  public:
    int n;

    x() : n(0) {}
  };

class y
  {
  public:
    int v;

    y() : v(0) {}
    static __thread x *xp;
  };

__thread x *y::xp;

static void
foo (y *yp)
{
  yp->v = 1;   /* foo_marker */
}

static void *
bar (void *unused)
{
  x xinst;
  y::xp= &xinst;

  y yy;
  foo(&yy);

  return NULL;
}

int
main(int argc, char *argv[])
{
  pthread_t t[2];

  pthread_create (&t[0], NULL, bar, NULL);
  pthread_create (&t[1], NULL, bar, NULL);

  pthread_join (t[0], NULL);
  pthread_join (t[1], NULL);

  return 0;
}
