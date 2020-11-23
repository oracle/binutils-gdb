#include <stdio.h>

extern __thread char bar[];
extern char  size_of_bar asm ("bar@SIZE");
extern void  set_bar (int, int);
char *       bar_size   = & size_of_bar;

int
main (void)
{
  set_bar (1, 20);
  if (10L == (long) bar_size && bar[1] == 20)
    printf ("OK\n");

  return 0;
}
