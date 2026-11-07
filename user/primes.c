#include "kernel/types.h"
#include "user/user.h"

int readn(int fd, void* buf, int n)
{
  int remaining = n;
  char *p = buf;
  while(remaining)
  {
    int increment = read(fd, buf, remaining);
    if(increment  < 0) return -1;
    if(!increment) return n - remaining; // EOF before getting all bytes
    remaining -= increment;
    p += increment;
  }
  return n;
}

int writen(int fd, void* buf, int n)
{
  int remaining = n;
  char *p = buf;

  while(remaining)
  {
    int increment = write(fd, p ,remaining);
    if(increment < 0) return -1;
    if(!increment) return n - remaining;  // unusual but possible
    remaining -= increment;
    p += increment;
  }
  return n;
}

void pipeline(int *p0)
{
  // close writing to parent
  close(p0[1]);

  int prime;
  // read from parent
  if(readn(p0[0], &prime, 4) < 4)
  {
    // close reading from parent
    close(p0[0]);
    return;
  }
  fprintf(1, "prime %d\n", prime);

  int p1[2];
  if(pipe(p1) < 0) exit(-1);
  if(fork() == 0)
  {
    pipeline(p1);
    close(p1[0]);
    exit(0);
  }

  close(p1[0]);
  int buf;
  // read from parent
  while(readn(p0[0], &buf, 4) == 4)
  {
    if(buf % prime == 0) continue; // dropping

    //write to child
    if(write(p1[1], &buf, 4) < 4)
    {
      close(p1[1]);
      close(p0[0]);
      return;
    }
  }
  // close writing to child
  close(p1[1]);
  // close reading from parent
  close(p0[0]);
  while(wait(0) > 0);
}


int main()
{
  int p[2];
  pipe(p);

  if(fork() == 0)
  {
    pipeline(p);
    close(p[0]);
    exit(0);
  }

  close(p[0]);
  for(int i = 2; i <= 35; i++)
  {
    if(writen(p[1], &i, 4) < 4) break;
  }
  close(p[1]);

  while(wait(0) > 0);
  exit(0);
}
