#include <stdlib.h>
#include <signlist.h>
#include <sx.h>

void
sx_syss_dump(FILE *fp, struct sl_signlist *sl)
{
  List *s;
  for (s = list_first(sl->syslists); s; s = list_next(sl->syslists))
    {
      struct sl_sys *sys;
      for (sys = list_first(s); sys; sys = list_next(s))
	{
	  struct sl_inst *ip = sys->ip;
	  if (ip)
	    {
	      const char *oid = NULL;
	      if (ip->type == 's')
		oid = ip->u.s->oid;
	      else if (ip->type == 'f')
		oid = ip->u.f->oid;
	      else
		fprintf(stderr, "@sys on sl_inst which is not sign or form\n");
	      if (oid)
		fprintf(fp, "%s\t%s\t%s\t%s\n", oid, sys->name, (sys->v ? (ccp)sys->v : ""), (sys->vv ? (ccp)sys->vv : ""));
	    }
	}
    }
  
}
