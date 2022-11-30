#include <stdio.h>

extern char  size_of_bar  asm ("bar@SIZE");
char *       bar_size   = & size_of_bar;

int
main (void)
{
  if (10L == (long) bar_size)
    printf ("OK\n");

  return 0;
}
