#include <oraccsys.h>

char *
normalize_ws(const char *s)
{
  while (isspace(*s))
    ++s;
  
  char buf[strlen(s)+1], *b = buf;
  
  while (*s)
    {
      if (isspace(*s))
	{
	  *b++ = ' ';
	  ++s;
	  while (isspace(*s))
	    ++s;
	}
      else
	*b++ = *s++;
    }
  while (isspace(b[-1]))
    --b;
  *b = '\0';

  return strdup(buf);
}
