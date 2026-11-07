#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

char *basename(char *path)
{
  char *p = path;
  char *last = path;
  // find the last '/'
  while (*p) {
    if (*p == '/')
      last = p + 1;
    p++;
  }
  return last;
}


void find(char *path, char *pattern)
{
  int fd;
  struct stat st;
  char buf[512], *p;
  struct dirent de;

  if((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type)
  {
  case T_FILE:

    if(strcmp(pattern, basename(path)) == 0)
    {
      fprintf(1, "%s\n", path);
    }
    break;

  case T_DIR:
    strcpy(buf, path);
    p = buf+strlen(buf);
    if (p > buf && *(p-1) != '/')   // only add '/' if not already present
        *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0) continue;
      if(strcmp(de.name, ".") == 0) continue;
      if(strcmp(de.name, "..") == 0) continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, pattern);
    }
    break;
  }
  close(fd);
}


int main(int argc, char *argv[])
{
  if(argc <= 1)
  {
    fprintf(2, "usage: find path pattern\n");
    exit(1);
  }
  if(argc == 2)
  {
    find(".", argv[1]);
    exit(0);
  }
  find(argv[1], argv[2]);
  exit(0);
}
