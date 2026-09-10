#include <oraccsys.h>
#include "vx.h"

extern int s_words;
#define ucun_x "\xE2\x96\x92"

void
vx_cun_node(Node *np, FILE *fp)
{
  Node *npp;
  const char *nodename = np->name;
  Prop *g_utf8_prop;

  if (!strcmp(nodename, "g:w"))
    {
      if (s_words++)
	fputc(' ', fp);
      for (npp = np->kids; npp; npp = npp->next)
	vx_cun_node(npp, fp);
    }
  else if ((g_utf8_prop = prop_find_kv(np->props, "g:utf8", NULL)))
    {
      fputs(g_utf8_prop->u.k->v, fp);
    }
  else if (!strcmp(np->name, "g:x")
	   || (!strcmp(np->name, "g:nonw") && !np->kids))
    {
      fputs(ucun_x, fp);
    }
  else
    {
      Node *cp;
      for (cp = np->kids; cp; cp = cp->next)
	vx_cun_node(cp, fp);
    }
}
