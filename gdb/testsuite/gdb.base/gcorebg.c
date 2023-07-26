#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

int main (int argc, char **argv)
{
  pid_t pid = 0;
  pid_t ppid;
  char buf[1024*2 + 500];
  int gotint;

  if (argc != 4)
    {
      fprintf (stderr, "Syntax: %s {standard|detached} <gcore command> <core output file>\n",
	       argv[0]);
      exit (1);
    }

  pid = fork ();

  switch (pid)
    {
      case 0:
        if (strcmp (argv[1], "detached") == 0)
	  setpgrp ();
	ppid = getppid ();
	gotint = snprintf (buf, sizeof (buf), "sh %s -o %s %d", argv[2], argv[3], (int) ppid);
	assert (gotint < sizeof (buf));
	system (buf);
	fprintf (stderr, "Killing parent PID %d\n", ppid);
	kill (ppid, SIGTERM);
	break;

      case -1:
	perror ("fork err\n");
	exit (1);
	break;

      default:
	fprintf (stderr,"Sleeping as PID %d\n", getpid ());
	sleep (60);
    }

  return 0;
}
