#include <oraccsys.h>

/* Punctuate the argument by replacing '.' with ' ' if the '.' is not
 * inside '(' ... ')'; pipes are removed if present.
 *
 * The work is done in-place; parens must be matching.
 */
unsigned char *
gdlseq(unsigned char *s)
{
  unsigned char *ret = s;
  int b = 0;
  if ('|' == *s)
    {
      size_t len = strlen((char*)s);
      memmove(s, s+1, len-2);
      s[len-2] = '\0';      
    }
  while (*s)
    {
      if ('(' == *s)
	++b;
      else if (')' == *s)
	--b;
      else if ('.' == *s && !b)
	*s = ' ';
      ++s;
    }
  return ret;
}
