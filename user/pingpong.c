#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char byte = 'c';
  char buf;
  int p1[2];
  int p2[2];
  pipe(p1);
  pipe(p2);

  if(fork() == 0)
  {
    // child:  read from parent p1[0], write to parent p2[1]
    close(p1[1]);
    close(p2[0]);

    if(read(p1[0], &buf, 1) < 0)
    {
      exit(1);
    }
    fprintf(1, "%d: received ping\n", getpid());

    if(write(p2[1], &buf, 1) < 0)
    {
      exit(1);
    }

    close(p1[0]);
    close(p2[1]);
    exit(0);
  }

  // parent: read from child p2[0], write to child p1[1]
  close(p2[1]);
  close(p1[0]);

  if(write(p1[1], &byte, 1) < 0)
  {
    exit(1);
  }

  if(read(p2[0], &buf, 1) < 0)
  {
    exit(1);
  }
  fprintf(1, "%d: received pong\n", getpid());
  exit(0);
}
