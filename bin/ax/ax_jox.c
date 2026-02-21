#include <oraccsys.h>
#include <joxer.h>
#include <rnvif.h>
#include <xmlify.h>
#include <pool.h>
#include <joxer.h>
#include <xnn.h>
#include <ns-xtf.h>
#include <rnvxml.h>
#include <gt.h>
#include <asl.h>
#include "ax.h"

Mloc *xo_loc;

const char *xfn = NULL;
const char *file;

static void
ax_jox_node(Node *np, int oflag, int nflag)
{
  Node *npp;
  int need_closer = 1;
  const char *nodename = np->name;

  joxer_ea(np->mloc, nodename,
	   rnvval_aa_ccpp(prop_ccpp(np->props, AP_ATTR, PG_XML)));

  if (np->text)
    {
      joxer_ch(np->mloc, np->text);
    }
  else
    {
      /* Descend recursively into child nodes */
      for (npp = np->kids; npp; npp = npp->next)
	ax_jox_node(npp, need_closer, 1);
    }
  joxer_ee(np->mloc, nodename);

#if 0
  /* traverse laterally along siblings */ /* not sure why gdl_jox needed this but ax_jox doesn't */q
  if (nflag && np->next)
    ax_jox_node(np->next, 0, 1);
#endif
}

void
ax_jox(Tree *tp)
{
  if (!xfn)
    xfn = "ax.xml";

  FILE *xfp = fopen(xfn, "w");

  jox_xml_output(xfp);
  joxer_init(&xtf_data, "xtf", 1, xfp, NULL);
  xo_loc = malloc(sizeof(Mloc));
  xo_loc->file = file = tp->root->mloc->file;
  xo_loc->line = 1;
  ax_jox_node(tp->root, 0, 1);
  joxer_term(xfp,NULL);
}
