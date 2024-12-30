#include <oraccsys.h>
#include <joxer.h>
#include <ns-numsys.h>
#include "ns.h"

static int ns_jx_init(void);
static void ns_jx_num(const char *tag, nx_num n);
static void ns_jx_step(ns_step *sp);

Mloc *xo_loc;
int rnvtrace = 0;

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
	  if (sp->alt)
	    {
	      joxer_ea(xo_loc, "nss:alt", NULL);
	      ns_jx_step(sp);
	      ns_step *ap;
	      for (ap = sp->alt; ap; ap = ap->next)
		ns_jx_step(ap);
	      joxer_ee(xo_loc, "nss:alt");
	    }
	  else
	    {
	      ns_jx_step(sp);
	    }
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
    xfn = "ns.xml";
  jfp = opt_json ? fopen(jfn, "w") : NULL;
  f_xml = xfp = opt_xml ? fopen(xfn, "w") : NULL;

  jox_jsn_output(jfp);
  jox_xml_output(xfp);
  joxer_init(&numsys_data, "numsys", 1, xfp, jfp);
  return 0;
}

static void
ns_jx_num(const char *tag, nx_num n)
{
  char nbuf[128];
  char dbuf[32];
  sprintf(nbuf, "%lld", n.n);
  sprintf(dbuf, "%d", n.d);
  ratts = rnvval_aa("x", "n", nbuf, "d", dbuf, NULL);
  joxer_ec(xo_loc, tag, ratts);
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
      list_add(a, sp->unit);
    }
  if (sp->axis)
    {
      list_add(a, "axis");
      list_add(a, (char*)sp->axis);
    }	    
  atts = list2chars(a);
  ratts = rnvval_aa_qatts((char**)atts, list_len(a)/2);
  list_free(a, NULL);
  joxer_ea(xo_loc, "nss:step", ratts);
  if (sp->aev.d)
    ns_jx_num("nss:aev", sp->aev);
  if (sp->mult.d)
    ns_jx_num("nss:mul", sp->mult);
  ns_inst *ip;
  for (ip = sp->insts; ip; ip = ip->next)
    {
      a = list_create(LIST_SINGLE);
      if (ip->text)
	{
	  list_add(a, "n");
	  list_add(a, (char*)ip->text);
	}
      if (ip->unit && *ip->unit != 1)
	{
	  list_add(a, "unit");
	  list_add(a, (char*)ip->unit);
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
	ns_jx_num("nss:aev", ip->aev);
      if (ip->count.d)
	ns_jx_num("nss:count", ip->count);
      joxer_ee(xo_loc, "nss:inst");
    }
  joxer_ee(xo_loc, "nss:step");
}
