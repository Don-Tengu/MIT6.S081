#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {

  if(argc < 2) {
    fprintf(2, "usage: (...arguments ->) xargs command ...arguments\n");
    exit(1);
  }


  int n = 0;
  char c;
  int base = argc - 1;

  // no arguments through pipe, argc - 1 arguments
  if((n = read(0, &c, 1)) == 0) {
    if(exec(argv[1], argv + 1) < 0) {
      fprintf(2, "exec failed\n");
      exit(1);
    }
  }

  if(n < 0) {
    fprintf(2, "Read Error!\n");
    exit(1);
  }

  char buf[512];
  buf[0] = c;
  char *p = buf + 1;

  // argc arguments
  char *xargv[argc + 1];
  memmove(xargv, argv + 1, base * sizeof(char *));
  xargv[argc] = 0;

  while((n = read(0, p, 1)) > 0) {
    if(p - buf == 511 && *p != '\n') {
      fprintf(2, "Too long argument\n");
      exit(1);
    }

    if(*p == '\n') {
      *p = 0;
      xargv[argc - 1] = buf;
      p = buf;
      if(fork() == 0) {
        if(exec(xargv[0], xargv) < 0) {
          fprintf(2, "exec failed\n");
          exit(1);
        }
      }
    } else {
      p++;
    }

  }

  if (n < 0) {
    fprintf(2, "Read Error!\n");
    exit(1);
  }

  while(wait(0) > 0);
  exit(0);
}
