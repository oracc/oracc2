#include <mesg.h>
#include "scan.h"

void
scan_square(Scan *s, unsigned char *endtok, unsigned char **text_start, 
	    size_t *local_lnump)
{
  size_t local_lnum = *local_lnump;
  if (*endtok == '[')
    {
      unsigned char *endattr = (unsigned char*)strchr((const char *)endtok,']');
      unsigned char *newline;
      if (endattr)
	{
	  *endattr = '\0';
	  s->attr = (ccp)++endtok;
	  if ((newline = (unsigned char *)strchr((const char *)endtok,'\n')))
	    {
	      while (newline)
		{
		  ++local_lnum;
		  if (newline[-1] == '\\')
		    newline[-1] = newline[0] = ' ';
		  else
		    mesg_err(s->np->mloc,"newline in attribute group (missing ']'?)");
		  while ('\n' == *newline)
		    ++newline;
		  newline = (unsigned char *)strchr((const char *)newline,'\n');
		}
	    }
	  *text_start = endattr+1;
	}
      else
	{
	  mesg_err(s->np->mloc,"missing ']' on attribute group");
	  /* error recovery: reset text_start to end of current line */
	  while (*endtok && '\n' != *endtok)
	    ++endtok;
	  *text_start = endtok;
	}
    }
  else
    *text_start = endtok;
  *local_lnump = local_lnum; 
}
