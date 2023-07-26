#include <stdio.h>
#include <unistd.h>

int
main (int argc, char **argv)
{
  if (fork () == 0)
    sleep (1);
  chdir (".");
  return 0;
}
