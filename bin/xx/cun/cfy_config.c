#include <oraccsys.h>
#include <tree.h>
#include <gdl.h>
#include "cfy.h"
#include "cfy.tab.h"

int cfy_cfg_status; /* set to 1 if there are parse errors for a sub */
static List *lhs, *rhs;

int
cfy_cfg_load(Cfy *c, const char *cfgfile)
{
  if (!access(cfgfile, R_OK))
    {
      FILE *cfp = fopen(cfgfile, "r");
      if (cfp)
	{
	  gdl_init();
	  gvl_setup("osl", "osl", "020");
	  c->m_subspec = memo_init(sizeof(Subspec), 1024);
	  c->hsubhead = hash_create(128);
	  c->hsubkeys = hash_create(128);
	  extern const char *currcfyfile;
	  extern FILE *cfyin;
	  currcfyfile = c->config;
	  cfyin = cfp;
	  cfyparse();
	  fclose(cfp);
	  gdl_term();
	  return cfy_cfg_status;
	}
      else
	{
	  mesg_verr(NULL, "failed to open config file %s", cfgfile);
	  return 1;
	}
    }
  else
    {
      mesg_verr(NULL, "config file %s not readable", cfgfile);
      return 1;
    }
}

const char *
elts_one_key(Elt *e)
{
  switch (e->etype)
    {
    case ELT_L:
      return "l";
    case ELT_C:
      return "c";
    case ELT_W:
      return "w";
    case ELT_G:
      return e->key;
    case ELT_J:
      return "j";
    case ELT_N:
      return "n";
    case ELT_F:
      return "f";
    case ELT_R:
      return "r";
    case ELT_X:
      return "x";
    case ELT_D:
      /* ignore deleted nodes */
      return NULL;
    case ELT_Q:
      return e->data;	  
    case ELT_LAST:
      return NULL; /* this can't happen */
    }
}

static char *
elts_key(Cfy *c, Elt **lelts, int n)
{
  List *klist = list_create(LIST_SINGLE);
  int i;
  for (i = 0; i < n; ++i)
    {
      const char *k = elts_one_key(lelts[i]);
      if (k)
	list_add(klist, (void*)k);
    }
  char *s = (char*)list_to_str2(klist, "_");
  char *p = (char*)pool_copy((uccp)s, c->p);
  free(s);
  list_free(klist, NULL);
  return p;
}

void
cfy_cfg_stash(Mloc m, Cfy *c)
{
  Subspec *sp = memo_new(c->m_subspec);
  sp->start = anchor_start;
  sp->end = anchor_end;
  anchor_start = anchor_end = 0;
  if (!cfy_cfg_status)
    {
      sp->l = (Elt**)list2array_c(lhs, &sp->l_len);
      sp->r = (Elt**)list2array_c(rhs, &sp->r_len);
      if (sp->l[0]->key)
	hash_add(c->hsubhead, (uccp)sp->l[0]->key, "");
      else
	{
	  mesg_verr(&m, "substitution must begin with a grapheme");
	  ++cfy_cfg_status;
	}
      hash_add(c->hsubkeys, (uccp)elts_key(c, sp->l, sp->l_len), sp);
    }
  else
    mesg_verr(&m, "substitution specification ignored because of errors\n");
    
  list_free(lhs, NULL);
  list_free(rhs, NULL);
  lhs = rhs = NULL;
}

static List *lhs = NULL, *rhs = NULL;

static Elt *
cfy_cfg_elt(Cfy *c, Etype e)
{
  extern int elts_rhs;
  if (!lhs)
    {
      lhs = list_create(LIST_SINGLE);
      rhs = list_create(LIST_SINGLE);
    }
  Elt *ep = memo_new(c->m_elt);
  list_add(elts_rhs ? rhs : lhs, ep);
  ep->etype = e;
  return ep;
}

void
cfy_cfg_elt_g(Mloc m, Cfy *c, uccp g)
{
  Elt *ep = cfy_cfg_elt(c, ELT_G);
  Tree *tp = gdlparse_string(&m, (char*)pool_copy(g,c->p));
  if (tp && tp->root && tp->root->kids)
    {
      uccp gkey = gvl_key(tp->root->kids->user, c->p);
      if (gkey)
	ep->key = (ccp)g;
      else
	++cfy_cfg_status;
    }
  else
    ++cfy_cfg_status;
}

void
cfy_cfg_elt_q(Mloc m, Cfy *c, uccp q)
{
  Elt *ep = cfy_cfg_elt(c, ELT_Q);
  ep->data = (void*)q;
}

void
cfy_cfg_elt_f(Mloc m, Cfy *c)
{
  (void)cfy_cfg_elt(c, ELT_F);
}

void
cfy_cfg_elt_r(Mloc m, Cfy *c)
{
  (void)cfy_cfg_elt(c, ELT_R);
}

void
cfy_cfg_elt_w(Mloc m, Cfy *c)
{
  (void)cfy_cfg_elt(c, ELT_W);
}

void
cfy_cfg_elt_j(Mloc m, Cfy *c)
{
  (void)cfy_cfg_elt(c, ELT_J);
}

void
cfy_cfg_elt_n(Mloc m, Cfy *c)
{
  (void)cfy_cfg_elt(c, ELT_N);
}

	      
