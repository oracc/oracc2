#include <oraccsys.h>

int
file_copy(const char *from, const char *to)
{
  FILE *fromfp = NULL;

  if ((fromfp = fopen(from, "r")))
    {
      FILE *tofp = NULL;
      if ((tofp = fopen(to, "w")))
	{
	  int ch;
	  while (EOF != (ch = fgetc(fromfp)))
	    fputc(ch,tofp);
	  fclose(fromfp);
	  fclose(tofp);
	  return 0;
	}
    }
  return 1;
}
