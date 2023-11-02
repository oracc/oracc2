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
		fprintf(stderr, "@sys on sl_inst which is not sign or form\n");
	      if (oid)
		{
		  const char *colon = "";
		  const char *subname = "";
		  if (sys->subname)
		    {
		      colon = ":";
		      subname = sys->subname;
		    }
		  fprintf(fp, "%s\t%s\t%s\t%s%s%s\t%s\t%s\n",
			  oid,
			  sys->ip->type == 's' ? "sign" : "form",
			  n,
			  sys->name,
			  colon,
			  subname,
			  (sys->v ? (ccp)sys->v : ""),
			  (sys->vv ? (ccp)sys->vv : ""));
		}
	    }
	}
    }
  
}
