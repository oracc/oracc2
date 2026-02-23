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
  else if (np->user)
    {
      struct axjoxfnc *ap = axjoxfnc(nodename,strlen(nodename));
      if (ap)
	ap->func(np, np->user);
      else
	fprintf(stderr, "ax_jox_node: internal error: no handler for tag '%s'\n",
		nodename);
    }
  else
    {
      /* Descend recursively into child nodes */
      for (npp = np->kids; npp; npp = npp->next)
	ax_jox_node(npp, need_closer, 1);
    }
  joxer_ee(np->mloc, nodename);

#if 0
  /* traverse laterally along siblings */ /* not sure why gdl_jox needed this but ax_jox doesn't */
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

/* handler functions for np->user and GDL */
void
ax_jox_bib(Node *np, Bib *p)
{
  joxer_ch(np->mloc, (ccp)p->text);
}

void
ax_jox_lang(Node *np, ATF *a)
{
  char buf[(a->altlang?strlen(a->altlang)+3:0)+strlen(a->lang)+2];
  if (a->altlang)
    sprintf(buf, "lang %s _%s_", a->lang, a->altlang);
  else
    sprintf(buf, "lang %s", a->lang);
  joxer_ch(np->mloc, buf);
}

void
ax_jox_note(Node *np, Note *p)
{
  joxer_ch(np->mloc, (ccp)p->text);
}

/* This is to handle atf: lang -- the void ptr is ATF * */
void
ax_jox_protocol(Node *np, void *p)
{
  Prop*ptype = prop_find_kv(np->props, "type", NULL);
  if (ptype && !strcmp(ptype->u.k->v, "bib"))
    ax_jox_bib(np, p);
  else if (ptype)
    ax_jox_lang(np, p);
}


