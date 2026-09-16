#include <oraccsys.h>
#include <gdl.h>
#include "vx.h"

Hash *h_catf = NULL;

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
    vx_atf_node(mp, fp);
  vxa_closers(np, fp);
  vxa_g_delim(np, fp);
}

void
vx_catf(Tree *tp, FILE *fp)
{
  
}
