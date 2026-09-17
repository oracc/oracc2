#include <oraccsys.h>
#include <gdl.h>
#include <roco.h>
#include "vx.h"

static Hash *h_catf = NULL;
static Roco *r_catf = NULL;

void
vxc_simples(Node *np, FILE *fp)
{
  vxa_openers(np, fp);
  if (np->text)
    {
      const char *catf = hash_find(h_catf, (uccp)np->text);
      if (catf)
	fputs(catf, fp);
      else
	{
	  mesg_verr(np->mloc, "text %s not in C-ATF map\n", np->text);
	  
	}
    }
  Node *mp;
  for (mp = np->kids; mp; mp = mp->next)
    vx_atf_gdl_node(mp, fp);
  vxa_closers(np, fp);
  vxa_g_delim(np, fp);
}

static void
vxc_load_map(void)
{
  char *catf_map = oracc_data("catf-map.tsv");
  r_catf = roco_load1(catf_map);
  h_catf = roco_hash(r_catf);
}

void
vx_catf(Tree *tp, FILE *fp)
{
  static CATF_helper ch;
  ch.outfp = fp;
  if (tp->root)
    {
      vxc_load_map();
      Node *start = tp->root;
      Vxcfnctab *vp = vxcfnctab(start->name, strlen(start->name));
      if (!vp)
	{
	  if (start->kids && (vp = vxcfnctab(start->kids->name, strlen(start->kids->name))))
	    start = start->kids;
	  else
	    {
	      if (start->kids)
		fprintf(stderr, "vx_catf: root node %s / root->kids %s not a C-ATF node. Stop.\n",
			start->name, start->kids->name);
	      else
		fprintf(stderr, "vx_catf: root node %s not a C-ATF node. Stop.\n",
			start->name);
	    }
	}
      else
	{
	  ch.start = start;
	  vp->fnc(start, &ch);
	}
    }
}
