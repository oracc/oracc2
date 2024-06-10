#include <oraccsys.h>
#include "tsv.h"

int
tsv_output(Tsv *tp)
{
  if ((tp->tsv_fp = fopen(tp->tsv_fn, "r")))
    {
      int n = 0;
      fseek(tp->tsv_fp, tp->data.seek, 0);
      while (n++ <= tp->data.len)
	{
	  int ch = fgetc(tp->tsv_fp);
	  if (ch == EOF)
	    {
	      /*fprintf(stderr, "xisdb: %s, seek=%ld, len=%d: ran out of bytes\n",
		tis_file, t_info.seek, t_info.len);*/
	      return 1;
	    }
	  else
	    {
#if 0
	      if (ch == ' ')
		fputc('\n', tp->out_fp);
	      else
#endif
		fputc(ch, tp->out_fp);
	    }
	}
      fclose(tp->tsv_fp);
    }
  return 0;
}
