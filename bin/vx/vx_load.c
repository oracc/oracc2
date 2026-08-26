#include <oraccsys.h>
#include "vx.h"

static const char *
vx_name(Hash *xtf_h, Tree *tp, const char *nam, nscode *codep)
{
  *codep = NS_NONE;
  char name[strlen(nam)+1]; strcpy(name,nam);
  const char *xtfname = NULL;
  NSdata *ns = NULL;
  char *ns_url = name;
  char *ln = strchr(name, '|');
  if (ln)
    {
      *ln++ = '\0';
      ns = nsdata(ns_url, strlen(ns_url));
      if (!ns)
	fprintf(stderr, "vx: internal error: no NSdata for %s\n", ns_url);
      *codep = ns->code;
    }
  else
    {
      ns_url = NULL;
    }
  if (ns)
    {
      char pnam[strlen(nam)+1];
      sprintf(pnam, "%s:%s", ns->equiv, ln);
      *codep = ns->code;
      xtfname = (ccp)hash_exists(xtf_h, (uccp)pnam);
      if (!xtfname)
	{
	  fprintf(stderr, "vx: xml error: %s not known in XTF schema\n", pnam);
	  xtfname = (ccp)pool_copy((uccp)pnam, tp->tm->pool);
	}
    }
  else
    {
      xtfname = (ccp)hash_exists(xtf_h, (uccp)ln);
      if (!xtfname)
	{
	  fprintf(stderr, "vx: xml error: %s not known in XTF schema\n", ln);
	  xtfname = (ccp)pool_copy((uccp)ln, tp->tm->pool);
	}
    }
  return xtfname;
}

void
vx_attr(Node *np, const char **atts)
{
  int i;
  for (i = 0; atts[i]; i += 2)
    {
      nscode nsc;
      const char *aname = vx_name(xtf_a, np->tree, atts[i], &nsc);
      prop_node_add(np, PROP_ANY, PG_XML, aname,
		    (ccp)hpool_copy((uccp)atts[i+1], np->tree->tm->pooh));
    }
}

void
vx_char(Tree *tp, const char *c)
{
  Node *np = tree_add(tp, NS_NONE, "#", tp->curr->depth+1, tree_mloc(tp, pi_file, pi_line));
  np->text = (ccp)pool_copy((uccp)c, tp->tm->pool);
}

static Node *
vx_push(Tree *tp, const char *nam)
{
  nscode nsc;
  const char *xtfname = vx_name(xtf_e, tp, nam, &nsc);
  (void)tree_add(tp, nsc, xtfname, tp->curr->depth+1, tree_mloc(tp, pi_file, pi_line));
  return tree_push(tp);
}

static void
vx_root(Tree *tp, const char *nam)
{
  nscode nsc;
  const char *xtfname = vx_name(xtf_e, tp, nam, &nsc);
  tree_root(tp, nsc, xtfname, 0, tree_mloc(tp, pi_file, pi_line));
}

static void
vx_sH(void *vp, const char *name, const char **atts)
{
  char *c = charData_retrieve();
  if (*c)
    vx_char(vp, c);
  Node *ep = vx_push(vp, name);
  if (atts[0])
    vx_attr(ep, atts);
}

static void
vx_eH(void *vp, const char *name)
{
  char *c = charData_retrieve();
  if (*c)
    vx_char(vp, c);
  tree_pop(vp);
}

static void
vx_sH_root(void *vp, const char *name, const char **atts)
{
  vx_root(vp, name);
  if (atts[0])
    vx_attr(((Tree*)vp)->root, atts);
  XML_SetElementHandler(curr_rip->parser, vx_sH, vx_eH);  
}

Tree *
vx_load(const char *fn)
{
  char const *fnlist[2];
  fnlist[0] = fn;
  fnlist[1] = NULL;
  vx_xtf_init();
  Tree *tp = tree_init();
  runexpat_omit_rp_wrap();
  runexpatNSuD(i_list, fnlist, vx_sH_root, vx_eH, "|", tp);
  return tp;
}
