#include <oraccsys.h>
#include "../cun/ligmax.h"

int
main(int argc, char *const *argv)
{
  size_t nb;
  char *b;
  while ((b = (char*)loadoneline(stdin, &nb)))
    {
      char *lig = strstr(b, "<-");
      if (lig)
	{
	  char u[LIG_MAX][5];
	  int i = 0;
	  lig += 3;
	  while ('u' == *lig)
	    {
	      if (i)
		fputc(' ', stdout);
	      ++lig;
	      memcpy(u[i++], utf8ify((unsigned int)strtol(lig,NULL,16)), 5);
	      int j;
	      for (j = 0; j < i; ++j)
		fputs(u[j], stdout);
	      lig = strchr(lig, '_');
	      if (lig)
		++lig;
	      else
		break;
	      if (i > LIG_MAX)
		{
		  fprintf(stderr, "oin2lig: ligature too long. Stop.\n");
		  exit(1);
		}
	    }
	  fputc('\n', stdout);
	}
    }
}
