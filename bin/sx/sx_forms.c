#include <oraccsys.h>
#include <signlist.h>
#include "sx.h"

static FILE *
sx_forms_file(struct sl_signlist *sl, char **fnamep)
{
  char buf[strlen(oracc())+strlen("//")+(2*strlen(sl->project))+strlen("/02pub/.frm0")];
  sprintf(buf, "%s/%s/02pub/%s.frm", oracc(), sl->project, sl->project);
  *fnamep = (char*)pool_copy((uchar *)buf, sl->p);
  return fopen(*fnamep, "w");
}

void
sx_forms(struct sl_signlist *sl)
{
  char *fname;
  FILE *fp = sx_forms_file(sl, &fname);
  if (fp)
    {
      int i;
      for (i = 0; i < sl->nsigns; ++i)
	{
	  if (sl->signs[i]->nforms)
	    fprintf(fp, "%s\n", sl->signs[i]->oid);
	}
      fclose(fp);
    }
  else
    {
      fprintf(stderr, "sx: unable to open %s to write forms data: %s.\n", fname, strerror(errno));
    }
}
