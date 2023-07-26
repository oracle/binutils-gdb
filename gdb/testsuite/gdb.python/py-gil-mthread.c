#include <stdio.h>
#include <unistd.h>

int
main (void)
{
  int i;
  for (i = 0; i < 10; i++)
    {
      sleep (1); /* break-here */
      printf ("Sleeping %d\n", i);
    }
}
