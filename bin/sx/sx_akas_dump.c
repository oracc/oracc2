#include <stdlib.h>
#include <signlist.h>
#include <sx.h>

void
sx_akas_dump(FILE *fp, struct sl_signlist *sl)
{
  const char **keys = hash_keys(sl->haka);
  int i;
  for (i = 0; keys[i]; ++i)
    {
      struct sl_inst *ip = hash_find(sl->haka, (uccp)keys[i]);
      if (ip)
	{
	  const char *oid = NULL;
	  const char *n = NULL;
	  if (ip->type == 's')
	    {
	      oid = ip->u.s->oid;
	      n = (ccp)ip->u.s->name;
	    }
	  else if (ip->type == 'f')
	    {
	      oid = ip->u.f->oid;
	      n = (ccp)ip->u.f->name;
	    }
	  else
	    fprintf(stderr, "@aka on sl_inst which is not sign or form\n");
	  if (oid)
	    {
	      /* This output is the same format as sx_syss_dump so the
		 two can be used together easily */
	      fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\n",
		      oid,
		      ip->type == 's' ? "sign" : "form",
		      n,
		      "aka",
		      keys[i],
		      "");
	    }
	}
    }
}
