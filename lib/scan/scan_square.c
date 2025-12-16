#include <mesg.h>
#include "scan.h"

void
scan_square(Scanseg *ssp, unsigned char *endtok, unsigned char **text_start)
{
  if (*endtok == '[')
    {
      unsigned char *endattr = (unsigned char*)strchr((const char *)endtok,']');
      unsigned char *newline;
      if (endattr)
	{
	  *endattr = '\0';
	  ssp->attr = (ccp)++endtok;
	  if ((newline = (unsigned char *)strchr((const char *)endtok,'\n')))
	    {
	      while (newline)
		{
		  if (newline[-1] == '\\')
		    newline[-1] = newline[0] = ' ';
		  else
		    mesg_err(&ssp->sp->mp,"newline in attribute group (missing ']'?)");
		  while ('\n' == *newline)
		    {
		      ++ssp->sp->mp.line;
		      ++newline;
		    }
		  newline = (unsigned char *)strchr((const char *)newline,'\n');
		}
	    }
	  *text_start = endattr+1;
	}
      else
	{
	  mesg_err(&ssp->sp->mp,"missing ']' on attribute group");
	  /* error recovery: reset text_start to end of current line */
	  while (*endtok && '\n' != *endtok)
	    ++endtok;
	  *text_start = endtok;
	}
    }
  else
    *text_start = endtok;
}
