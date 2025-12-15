#include <ctype128.h>
#include <string.h>
#include "scan.h"

int
block_peek(Scan *sp, unsigned char *p, scan_block_tester f)
{
  unsigned char save, *tokp,*endp;
  int ret = 0;
  tokp = scan_token(sp,p,&endp,&save);
  if (f(tokp))
    ret = 1;
  *endp = save;
  return ret;
}

void
scan_text(Scanseg *ssp, unsigned char *text_start, 
	  unsigned char **text_end, scan_block_tester f)
{
  unsigned char *end=NULL, *trailing_white;
  switch (ssp->term)
    {
    case SCAN_WHITE: /* end at token */
      while (' ' == *text_start || '\t' == *text_start)
	++text_start;
      end = text_start;
      break;
    case SCAN_LINE: /* end at first newline */
      while (' ' == *text_start || '\t' == *text_start)
	++text_start;
      if (!ssp->np || !ssp->np->text)
	{
	  ssp->np->text = (char*)text_start;
	  end = text_start;
	}
      else
	end = text_start;
      while (*end && '\n' != *end)
	++end;
      trailing_white = end;
      while (trailing_white > text_start && isspace(trailing_white[-1]))
	--trailing_white;
      if (trailing_white != end)
	*trailing_white = '\0';
      if (*end)
	{
	  ++ssp->sp->mp->line;
	  *end++ = '\0';
	}
      break;
    case SCAN_PARA: /* end at blank line or cdt block token */
      while (' ' == *text_start || '\t' == *text_start)
	++text_start;
      ssp->np->text = (char*)text_start;
      end = text_start;
      while (*end)
	{
	  if ('\n' == *end)
	    {
	      unsigned char *start_of_end = end;
	      ++end;
	      ++ssp->sp->mp->line;
	      while (' ' == *end || '\t' == *end)
		++end;
	      if ('\n' == *end) /* blank line */
		{
		  *start_of_end = '\0';
		  ++ssp->sp->mp->line;
		  ++end;
		  break;
		}
	      else if ('@' == *end && block_peek(ssp->sp,end,f))
		{
		  ++ssp->sp->mp->line;
		  trailing_white = start_of_end;
		  while (trailing_white > text_start && isspace(trailing_white[-1]))
		    --trailing_white;
		  if (trailing_white != start_of_end)
		    *trailing_white = '\0';
		  else
		    *start_of_end = '\0'; /* zero out the newline */
		  goto ret;
		}
	    }
	  else
	    ++end;
	}
      break;
    case SCAN_END:
      ssp->np->text = (char *)text_start;
      end = text_start;
      while (*end)
	{
	  if ('\n' == *end)
	    {
	      ++ssp->sp->mp->line;
	      if (!strncmp((const char *)end+1,"@end",4))
		{
		  unsigned char *start_of_end = end;
		  int namelen = strlen(ssp->np->name);
		  end += 5;
		  while (' ' == *end || '\t' == *end)
		    ++end;
		  if (!strncmp((const char *)end,ssp->np->name,namelen) 
		      && (isspace(end[namelen]) || !end[namelen]))
		    {
		      *start_of_end = '\0';
		      end += namelen;
		      while (isspace(*end))
			if ('\n' == *end++)
			  ++ssp->sp->mp->line;
		      goto ret;
		    }
		}
	    }
	  ++end;
	}
      break;
    default:
      break;
    }
 ret:
  *text_end = end;
  return;
}
