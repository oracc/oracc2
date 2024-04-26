#include <oraccsys.h>

/* There is also a filecopy in futil but that one needs work so it
   doesn't just exit on error */
int
file_copy(const char *from, const char *to)
{
  FILE *fromfp = NULL;

  if ((fromfp = fopen(from, "r")))
    {
      FILE *tofp = NULL;
      if (!strcmp(to, "-"))
	tofp = stdout;
      if (tofp || (tofp = fopen(to, "w")))
	{
	  int ch;
	  while (EOF != (ch = fgetc(fromfp)))
	    fputc(ch,tofp);
	  fclose(fromfp);
	  if (strcmp(to, "-"))
	    fclose(tofp);
	  return 0;
	}
    }
  return 1;
}
