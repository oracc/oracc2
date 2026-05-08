#include <oraccsys.h>
#include "tsv.h"

int
tsv_output(Tsv *tp)
{
  if ((tp->tsv_fp = fopen(tp->tsv_fn, "r")))
    {
      int i;
      for (i = 0; i < tp->nkey; ++i)
	{
	  int n = 0;
	  fseek(tp->tsv_fp, tp->data[i].seek, 0);
	  while (n++ < tp->data[i].len)
	    {
	      int ch = fgetc(tp->tsv_fp);
	      if (ch == EOF)
		{
		  return 1;
		}
	      else
		{
		  if (ch == 0x0B || (' ' == ch && space_newlines))
		    fputc('\n', tp->out_fp);
		  else
		    fputc(ch, tp->out_fp);
		}
	    }
	}
      fclose(tp->tsv_fp);
    }
  return 0;
}
