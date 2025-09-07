#include <oraccsys.h>
#include <stddef.h>
#include <tree.h>
#include <gdl.h>
#include "cfy.h"
#include "cfy.tab.h"

int cfy_cfg_status; /* set to 1 if there are parse errors for a sub */
static List *lhs, *rhs;
extern int elts_rhs;
int sub_has_assignment;

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
    case ELT_NOT:
      return NULL;
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
    case ELT_A:
      /* only used in subbing rule rhs */
      return NULL;	  
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

static Subspec *
sp_clone(Cfy *c, Subspec *sp)
{
  Subspec *clone = memo_new(c->m_subspec);
  *clone = *sp;
  return clone;
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
      if (sub_has_assignment)
	{
	  sp->has_assignment = sub_has_assignment;
	  sub_has_assignment = 0;
	}
      if (!sp->l[0]->key)
	{
	  mesg_verr(&m, "substitution must begin with a grapheme");
	  ++cfy_cfg_status;
	}
      else
	{
	  /* check that this isn't a duplicate */
	  const char *ek = elts_key(c, sp->l, sp->l_len);
	  Subspec *hsp = hash_find(c->hsubkeys, (uccp)ek);
	  if (hsp && !hsp->terminal)
	    {
	      mesg_verr(&m, "duplicate left subspec %s\n", ek);
	      ++cfy_cfg_status;
	    }
	  else
	    {
	      /* add subpaths, not overwriting any previous terminal subpaths */
	      char buf[strlen(ek)+1];
	      strcpy(buf, ek);
	      char *uscore;
	      sp->terminal = 0;
	      while ((uscore = strrchr(buf, '_')))
		{
		  *uscore = '\0';
		  /* If subpath is in hash then hsp is either subpath
		     or terminal; since buf is also a subpath and we
		     wouldn't want to override a terminal with a
		     subpath we don't need to update hsp with a new
		     Subspec clone here */
		  if (!(hsp = hash_find(c->hsubkeys, (uccp)buf)))
		    {
		      if (trace)
			fprintf(stderr,
				"trace: cfy_cfg_stash() adding subpath %s for key %s\n",
				buf, ek);
		      /* This is a new subpath; we need to clone the
			 Subspec to ensure sp->terminal stays 0 */
		      hash_add(c->hsubkeys, (uccp)hpool_copy((uccp)buf,c->hp), sp_clone(c, sp));
		    }
		  else if (trace)
		    fprintf(stderr,
			    "trace: cfy_cfg_stash() subpath %s for key %s is a terminal\n",
			    buf, ek);
		}
	      sp->terminal = 1;
	      hash_add(c->hsubkeys, (uccp)ek, sp);
	      if (trace)
		fprintf(stderr,
			"trace: cfy_cfg_stash() adding terminal %s\n",
			ek);
	    }
	}
    }
  else
    mesg_verr(&m, "substitution specification ignored because of errors\n");
    
  list_free(lhs, NULL);
  list_free(rhs, NULL);
  lhs = rhs = NULL;
}

static List *lhs = NULL, *rhs = NULL;

void
cfy_cfg_asgn(Mloc m, Cfy *c, int nth, const char *memb, const char *val)
{
  if (elts_rhs)
    {
      Assignment *ap = memo_new(c->m_assignment);
      Elt *ep = memo_new(c->m_elt);
      ep->etype = ELT_A;
      ep->data = ap;
      list_add(rhs, ep);
      sub_has_assignment = 1;
      ap->lindex = nth;
      if (memb)
	{
	  struct subtok *sp = subtok(memb, strlen(memb));
	  if (sp)
	    {
	      if (sp->toktype == T_OFF)
		ap->offof = sp->memb_or_val;
	      else
		ap->offof = UINTPTR_MAX;
	      if (sp->valtype == V_STR)
		ap->value = (char*)val;
	      else
		{
		  sp = subtok(val, strlen(val));
		  if (sp)
		    {
		      if (ap->offof != UINTPTR_MAX)
			{
			  if (ap->offof == offsetof(Elt,etype) && sp->toktype != T_ELT)
			    {
			      mesg_verr(&m, "%s is not a known ELT token", val);
			      ++cfy_cfg_status;
			    }
			  else if (ap->offof == offsetof(Elt,btype) && sp->toktype != T_BRK)
			    {
			      mesg_verr(&m, "%s is not a known BRK token", val);
			      ++cfy_cfg_status;
			    }
			  else if (ap->offof == offsetof(Elt,gtype) && sp->toktype != T_G)
			    {
			      mesg_verr(&m, "%s is not a known G token", val);
			      ++cfy_cfg_status;
			    }
			  else
			    ap->value = (uintptr_t*)sp->memb_or_val;
			}
		      else
			ap->value = (uintptr_t*)sp->memb_or_val;
		    }
		  else
		    {
		      mesg_verr(&m, "%s is not a known subbing token", val);
		      ++cfy_cfg_status;
		    }
		}
	    }
	}
      else
	{
	  mesg_verr(&m, "%s is not a member of Elt", memb);
	  ++cfy_cfg_status;
	}
    }
  else
    {
      mesg_verr(&m, "assignments only allowed after =>");
      ++cfy_cfg_status;
    }
}

static Elt *
cfy_cfg_elt(Cfy *c, Etype e)
{
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
      gvl_g *gp = tp->root->kids->user;
      uccp gkey = gvl_key(gp, c->p);
      if (gkey)
	{
	  ep->key = (ccp)gkey;
	  ep->data = (void*)gvl_cuneify_gv(gp);
	  ep->oid = gp->oid;
	}
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

	      
