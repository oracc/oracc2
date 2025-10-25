#include "scan.h"
char *
scan_curly(char *p, char **endp)
{
  char *start;
  int nesting = 0;
  *p++ = '\0';
  start = p;
  while (*p)
    {
      if ('{' == *p && '\\' != p[-1])
	++nesting;
      else if ('}' == *p && '\\' != p[-1])
	{
	  if (nesting > 0)
	    --nesting;
	  else
	    {
	      *p++ = '\0';
	      break;
	    }
	}
      ++p;
    }
  if (endp)
    *endp = p;
  return start;
}
