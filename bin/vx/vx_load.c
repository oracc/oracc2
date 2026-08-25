#include <oraccsys.h>
#include "vx.h"

void
vx_char(Tree *tp, const char *c)
{
  Node *np = tree_add(tp, NS_NONE, "#", tp->curr->last->depth, NULL);
  np->text = (ccp)pool_copy((uccp)c, tp->tm->pool);
}

void
vx_push(Tree *tp, const char *nam)
{
  char name[strlen(nam)+1]; strcpy(name,nam);
  const char *xtfname = NULL;
  NSdata *ns = NULL;
  nscode nsc = NS_NONE;
  char *ns_url = name;
  char *ln = strchr(name, '|');
  if (ln)
    {
      *ln++ = '\0';
    }
  else
    {
      ns_url = NULL;
      ln = name;
      ns = nsdata(ns_url, strlen(ns_url));
      if (!ns)
	fprintf(stderr, "vx: internal error: no NSdata for %s\n", ns_url);
      nsc = ns->code;
    }
  if (ns)
    {
      char pnam[strlen(nam)+1];
      sprintf(pnam, "%s:%s", ns->equiv, ln);
      nsc = ns->code;
      xtfname = (ccp)hash_exists(xtf_e, (uccp)pnam);
      if (!xtfname)
	{
	  fprintf(stderr, "vx: xml error: %s not known in XTF schema\n", pnam);
	  xtfname = (ccp)pool_copy((uccp)pnam, tp->tm->pool);
	}
    }
  else
    xtfname = (ccp)pool_copy((uccp)ln, tp->tm->pool);

  (void)tree_add(tp, nsc, xtfname, tp->curr->last->depth, NULL);
  (void)tree_push(tp);
}

void
vx_sH(void *vp, const char *name, const char **atts)
{
  char *c = charData_retrieve();
  if (*c)
    vx_char(vp, c);
  vx_push(vp, name);
}

void
vx_eH(void *vp, const char *name)
{
  char *c = charData_retrieve();
  if (*c)
    vx_char(vp, c);
  tree_pop(vp);
}

Tree *
vx_load(const char *fn)
{
  char const *fnlist[2];
  fnlist[0] = fn;
  fnlist[1] = NULL;
  Tree *tp = tree_init();
  runexpat_omit_rp_wrap();
  runexpatNSuD(i_list, fnlist, vx_sH, vx_eH, "|", tp);
  return tp;
}
