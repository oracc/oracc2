#include <oraccsys.h>
#include <joxer.h>
#include <ns-numsys.h>
#include "ns.h"
#include "nx.h"

static int ns_jx_init(void);
static void ns_jx_num(const char *tag, nx_num n, const char *u);
static void ns_jx_step(ns_step *sp);

extern Mloc *xo_loc;
int rnvtrace = 0;
int frac_mode = 0;

extern int opt_json, opt_xml;
extern const char *jfn, *xfn, *file;
static FILE *jfp, *xfp;
FILE *f_xml = NULL;

extern struct rnvval_atts *ratts;

int
ns_jx(Hash *hsys, List *lsys)
{
  int res = 0;
  if ((res = ns_jx_init()))
    return res;
  
  ratts = rnvval_aa("x", "project", "numsys", "file", file, NULL);
  joxer_ea(xo_loc, "nss:systems", ratts);

  const char *sysname;
  for (sysname = list_first(lsys); sysname; sysname = list_next(lsys))
    {
      ns_sys *sysp = hash_find(hsys, (uccp)sysname);
      xo_loc->line = sysp->lnum;
      List *a = list_create(LIST_SINGLE);
      list_add(a, "name");
      list_add(a, sysp->name);
      if (sysp->base)
	{
	  list_add(a, "base");
	  list_add(a, sysp->base);
	}
      if (sysp->conv)
	{
	  list_add(a, "conv");
	  list_add(a, sysp->conv);
	}
      const char **atts = list2chars(a);
      list_free(a, NULL);
      ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
      joxer_ea(xo_loc, "nss:sys", ratts);
      ns_step *sp;
      for (sp = sysp->steps; sp; sp = sp->next)
	{
#if 0 /* This can't happen with new sp->fracs architecture */
	  if (sp->type == nxt_nf)
	    {
	      ratts = rnvval_aa("x", "unit", "/", "type", "frac", NULL);
	      joxer_ea(xo_loc, "nss:step", ratts);
	      ns_jx_step(sp);
	      if (sp->alt)
		{
		  ns_step *ap;
		  for (ap = sp->alt; ap; ap = ap->next)
		    ns_jx_step(ap);
		}
	      joxer_ee(xo_loc, "nss:step");
	    }
	  else
	    {
	      ns_jx_step(sp);
	    }
#endif
	  ns_jx_step(sp);
	}
      joxer_ee(xo_loc, "nss:sys");
    }

  joxer_ee(xo_loc, "nss:systems");
  
  joxer_term(xfp,jfp);
  return 0;
}

static int
ns_jx_init(void)
{
  xo_loc = malloc(sizeof(Mloc));

  if (!jfn)
    jfn = "ns.jsn";
  if (!xfn)
    xfn = "numsys.xml";
  jfp = opt_json ? fopen(jfn, "w") : NULL;
  f_xml = xfp = opt_xml ? fopen(xfn, "w") : NULL;

  jox_jsn_output(jfp);
  jox_xml_output(xfp);
  joxer_init(&numsys_data, "numsys", 1, xfp, jfp);
  return 0;
}

static void
ns_jx_num(const char *tag, nx_num n, const char *u)
{
  char nbuf[128];
  char dbuf[32];
  sprintf(nbuf, "%lld", n.n);
  sprintf(dbuf, "%d", n.d);
  if (u)
    ratts = rnvval_aa("x", "n", nbuf, "d", dbuf, "unit", u, NULL);
  else
    ratts = rnvval_aa("x", "n", nbuf, "d", dbuf, NULL);
  joxer_ec(xo_loc, tag, ratts);
}

static const char *
axis_ucun(const char *a)
{
  if (*a == 'A')
    {
      if (a[1])
	return "𒕐";
      else
	return "𒀸";	  
    }
  else
    {
      if (a[1])
	return a[2] == 'c' ? "𒕙" : "𒀹";
      else
	return "𒁹";
    }
  return NULL;
}

static void
ns_jx_step(ns_step *sp)
{
  List *a;
  const char **atts;
  
  a = list_create(LIST_SINGLE);
  if (sp->unit)
    {
      list_add(a, "unit");
      if (frac_mode)
	{
	  char buf[strlen((ccp)sp->unit)+3];
	  sprintf(buf, "/-%s", sp->unit);
	  list_add(a, pool_copy((uccp)buf, nxp->p));
	}
      else
	list_add(a, sp->unit);
    }
  if (!frac_mode)
    {
      if (sp->axis)
	{
	  list_add(a, "axis");
	  list_add(a, (char*)sp->axis);
	  const char *ac = axis_ucun(sp->axis);
	  if (ac)
	    {
	      list_add(a, (char*)"axis-ucun");
	      list_add(a, (char*)ac);
	    }
	}
      if (sp->type == nxt_nw)
	{
	  list_add(a, "type");
	  list_add(a, "word");
	  if (sp->nwp)
	    {
	      list_add(a, "cgp");
	      list_add(a, (char*)sp->nwp->cgp);
	      list_add(a, "oid");
	      list_add(a, (char*)sp->nwp->oid);
	      list_add(a, "ucun");
	      list_add(a, (char*)sp->nwp->ucun);
	    }
	}
      else if (sp->type == nxt_nb || sp->type == nxt_ng)
	{
	  list_add(a, "type");
	  list_add(a, "sign");
	}
    }
  else
    {
      list_add(a, "type");
      list_add(a, "frac");
    }    

  atts = list2chars(a);
  ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
  list_free(a, NULL);
  joxer_ea(xo_loc, "nss:step", ratts);
  if (!frac_mode)
    {
      if (sp->aev.d)
	{
	  ns_jx_num("nss:aev", sp->aev, NULL);
	  joxer_et(xo_loc, "nss:mev", NULL, nx_step_mev(sp));
	}
      if (sp->mult.d)
	ns_jx_num("nss:mul", sp->mult, NULL);
    }
  ns_inst *ip;
  for (ip = frac_mode ? sp->fracs : sp->insts; ip; ip = ip->next)
    {
      a = list_create(LIST_SINGLE);
      if (ip->text)
	{
	  list_add(a, "n");
	  list_add(a, (char*)ip->text);
	}
      if (ip->axis)
	{
	  list_add(a, "axis");
	  list_add(a, (char*)ip->axis);
	}
      atts = list2chars(a);
      ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
      list_free(a, NULL);
      joxer_ea(xo_loc, "nss:inst", ratts);
      if (ip->aev.d)
	ns_jx_num("nss:aev", ip->aev, NULL);
      if (ip->count.d)
	ns_jx_num("nss:count", ip->count, (ccp)ip->unit);
      joxer_ee(xo_loc, "nss:inst");
    }
  joxer_ee(xo_loc, "nss:step");
  if (sp->fracs && !frac_mode)
    {
      frac_mode = 1;
      ns_jx_step(sp);
      frac_mode = 0;
    }
}
