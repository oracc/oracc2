#include <oraccsys.h>
#include <px.h>

int
px_file_copy(Isp *ip, const char *fn, const char *to)
{
  if (access(fn, R_OK))
    {
      printf("<span class=\"error\"> "
	     PX_ERROR_START
	     " px_file_copy: file %s non-existent or unreadable</span>\n", fn);
      return 1;
    }
  file_copy(fn, to);
  return 0;
}

/* There is also a filecopy in futil but that one needs work so it
   doesn't just exit on error */
int
file_copy_lines(const char *from, const char *to, int start, int end)
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
	  int n = 1;
	  int w = 0;
	  w = (n >= start && n <= end);
	  while (EOF != (ch = fgetc(fromfp)))
	    {
	      if (w)
		fputc(ch,tofp);
	      if ('\n' == ch)
		{
		  ++n;
		  if (n > end)
		    break;
		  else if (n > start)
		    w = 1;
		}
	    }
	  fclose(fromfp);
	  if (strcmp(to, "-"))
	    fclose(tofp);
	  return 0;
	}
      else
	return 2;
    }
  return 1;
}
