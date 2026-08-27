#include <oraccsys.h>
#include "vx.h"

static Hash *seen_h = NULL;

static const char *
vx_name(Hash *xtf_h, Tree *tp, const char *name, nscode *codep)
{
  *codep = NS_NONE;
  const char *xtfname = NULL;
  NSdata *ns = NULL;
  const char *ns_url = name;
  const char *ln = strrchr(name, ':');
  if (ln)
    {
      int len = ln - ns_url;
      char ns_buf[len];
      strncpy(ns_buf, ns_url, len);
      ns_buf[len] = '\0';
      ns = nsdata(ns_buf, ln - ns_url);
      if (!ns)
	{
	  if (!hash_find(seen_h, (uccp)ns_buf))
	    {
	      fprintf(stderr, "%s:%ld: (vx) xml error: no NSdata for %s\n",
		      runexpat_file(), runexpat_lnum(), ns_buf);
	      hash_add(seen_h, pool_copy((uccp)ns_url, tp->tm->pooh), "");
	    }
	  goto bad_ns_url;
	}
      else
	{
#if 1
	  xtfname = (ccp)hash_find(xtf_h, (uccp)name);
#else
	  char pnam[strlen(name)+2];
	  sprintf(pnam, "%s:%s", ns->equiv, ln);
	  xtfname = (ccp)hash_exists(xtf_h, (uccp)pnam);
#endif
	  *codep = ns->code;
	  if (!xtfname)
	    {
	      if (!(xtfname = (ccp)hash_exists(seen_h, (uccp)name)))
		{
		  fprintf(stderr, "%s:%ld: (vx) xml error: %s not known in XTF schema\n",
			  runexpat_file(), runexpat_lnum(), ++ln);
		  xtfname = (ccp)pool_copy((uccp)name, tp->tm->pooh);
		  hash_add(seen_h, (uccp)xtfname, "");
		}
	    }
	}
    }
  else
    {
      ln = ns_url;
      ns_url = NULL;
      xtfname = (ccp)hash_exists(xtf_h, (uccp)ln);
      if (!xtfname)
	{
	bad_ns_url:
	  if (!(xtfname = (ccp)hash_exists(seen_h, (uccp)ln)))
	    {
	      xtfname = (ccp)pool_copy((uccp)ln, tp->tm->pooh);
	      fprintf(stderr, "%s:%ld: (vx) xml error: %s not known in XTF schema\n",
		      runexpat_file(), runexpat_lnum(), ln);
	      hash_add(seen_h, (uccp)xtfname, "");
	    }
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
  seen_h = hash_create(100);
  Tree *tp = tree_init();
  tree_ns_default(tp, NS_XTF);
  tree_ns_declare(tp, NS_ATF);
  runexpat_omit_rp_wrap();
  runexpatNSuD(i_list, fnlist, vx_sH_root, vx_eH, ":", tp);
  hash_free(seen_h, NULL);
  return tp;
}
