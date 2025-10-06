#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int
main(int argc, char **argv)
{
  if (argv[1] && !strcmp(argv[1], "check"))
    {
      int ch, lastch;
      while (EOF != (ch = getchar()))
	if (isspace(ch))
	  exit(lastch=='.' ? 0 : 1);
	else
	  lastch = ch;
    }
  else
    {
      int ch;
      int lnum = 1;
      while (EOF != (ch = getchar()))
	{
	  printf("%d. ", lnum++);
	  putchar(ch);
	  while (EOF != (ch = getchar()))
	    {
	      putchar(ch);
	      if ('\n' == ch)
		break;
	    }
	}
    }
}
