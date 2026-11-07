#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc == 0)
  {
    fprintf(2, "usage: sleep [seconds]\n");
    exit(1);
  }
  int ticks;
  if((ticks = atoi(argv[1])))
  {
    sleep(ticks);
  }

  exit(0);
}
