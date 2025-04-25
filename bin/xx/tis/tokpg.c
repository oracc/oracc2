#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BSIZ (8192*8)
char *buf, *p;

int
main(int argc, char **argv)
{
  char P[10];
  buf = malloc(BSIZ);
  while ((p = fgets(buf, BSIZ, stdin)))
    {
      if ('T' == *p)
	{
	  p = strchr(p, '\t');
	  p = strchr(p+1, '\t');
	  strncpy(P,p+1,7);
	  P[7] = '\0';
	}
      else if ('g' == *p)
	{
	  printf("%s\t%s",P,p);
	}
    }
}
