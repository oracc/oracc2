#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#define TM_MAX 256

int
main(int argc, char **argv)
{
  char tmbuf[TM_MAX];
  time_t t;
  struct tm *gmt;
  if (argc > 2) /* -f FILENAME */
    {
      struct stat st;
      if (!stat(argv[2], &st))
	{
#ifdef st_mtime
	  gmt = gmtime(&st.st_mtime);
#else
	  gmt = gmtime(&st.st_mtimespec.tv_sec);
#endif
	}
      else
	{
	  fprintf(stderr, "isogmt: stat failed on %s\n", argv[2]);
	  exit(1);
	}
    }
  else
    {
      t = time(NULL);
      gmt = gmtime(&t);
    }
  if (argc > 1)
    {
      if (!strcmp(argv[1], "-t"))
	strftime(tmbuf, TM_MAX, "%Y-%m-%dT%H:%M:%S+0000",gmt);
      else if (!strcmp(argv[1], "-s"))
	sprintf(tmbuf, "%ld",(long)t);
    }
  else
    strftime(tmbuf, TM_MAX, "%Y-%m-%d",gmt);;
  fputs(tmbuf, stdout);
  return 0;
}
