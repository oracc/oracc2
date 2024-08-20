#include <stdio.h>

int delim = ',';
int quote = '"';
int instr = 0;

int
main(int argc, char **argv)
{
  int ch;
  while (EOF != (ch = fgetc(stdin)))
    {
      if (instr)
	{
	  if (ch == quote)
	    {
	      if (EOF != (ch = fgetc(stdin)))
		{
		  if (ch == quote)
		    putchar('"');
		  else
		    {
		      instr = 0;
		      if (ch == delim)
			putchar('\t');
		      else
			putchar(ch);
		    }
		}
	      
	    }
	  else if (ch == '\n')
	    putchar('\v');
	  else if (ch == '\t')
	    putchar('\a');
	  else
	    putchar(ch);
	}
      else
	{
	  if (ch == delim)
	    putchar('\t');
	  else if (ch == quote)
	    instr = 1;
	  else
	    putchar(ch);
	}
    }	 
}
