#include <oraccsys.h>
#include <stddef.h>
#include <tree.h>
#include <gdl.h>
#include "cfy.h"
#include "cfy.tab.h"

int cfy_cfg_status; /* set to 1 if there are parse errors for a sub */
static List *lhs, *rhs, *cfy_col_units;
extern int elts_rhs;
int sub_has_assignment;

static const char *
cfy_cfg_locate(Cfy *c, const char *ccf, Ltype cloc)
{
  if (!ccf)
    return NULL;
  
  if (cloc == LOC_ARG)
    {
      return ccf; /* simple access, let cfy_cfg_load handle it */
    }
  else if (cloc == LOC_PRJ)
    {
      if (c->project)
	{
	  /* for project, look in project/00lib/cfy.d then in system */
	  char path[strlen(oracc())+strlen(c->project)+strlen("//00lib/0")+strlen(ccf)];
	  sprintf(path, "%s/%s/00lib/cfy.d/%s", oracc(), c->project, ccf);
	  if (!access(path, R_OK))
	    return (ccp)pool_copy((uccp)path,c->p);
	  if (c->proxy)
	    {
	      char path2[strlen(oracc())+strlen(c->proxy)+strlen("//00lib/0")+strlen(ccf)];
	      sprintf(path2, "%s/%s/00lib/%s", oracc(), c->proxy, ccf);
	      if (!access(path2, R_OK))
		return (ccp)pool_copy((uccp)path2,c->p);
	    }
	  char path3[strlen(oracc())+strlen("/lib/data/0")+strlen(ccf)];
	  sprintf(path3, "%s/lib/data/%s", oracc(), ccf);
	  if (!access(path3, R_OK))
	    return (ccp)pool_copy((uccp)path3,c->p);
	}
    }
  else if (cloc == LOC_TXT)
    {
      /* look in text dir, then try project places */
      extern char *inpath;
      char path[strlen(inpath)+1];
      strcpy(path, inpath);
      char *dot = strrchr(path,'.');
      if (dot) /* should always be true because .xtf input */
	{
	  strcpy(dot, ".ccf");
	  if (!access(path, R_OK))
	    return (ccp)pool_copy((uccp)path, c->p);
	  char *p = expand(c->project, c->pqx, "ccf");
	  if (strcmp(path, p) && !access(p, R_OK))
	    return (ccp)pool_copy((uccp)p, c->p);
	  if (c->proxy)
	    {
	      p = expand(c->proxy, c->pqx, "ccf");
	      if (!access(p, R_OK))
		return (ccp)pool_copy((uccp)p, c->p);
	    }
	}
      return cfy_cfg_locate(c, ccf, LOC_PRJ);
    }
  return ccf; /* default return allows cfy_cfg_load to report missing file */
}

int
cfy_cfg_run(Cfy *c)
{
  if (c->arg_ccf)
    return cfy_cfg_load(c, cfy_cfg_locate(c, c->arg_ccf, LOC_ARG));
  else if (!c->infile)
    return 0;
  else
    {
      /* Look for a .ccf in the same place as the .xtf */
      char buf[strlen(c->infile)+1];
      strcpy(buf, c->infile);
      strcpy(buf+strlen(buf)-3, "ccf");
      if (!access(buf, R_OK))
	c->text_ccf = (ccp)pool_copy((uccp)buf,c->p);

      if (c->text_ccf)
	return cfy_cfg_load(c, cfy_cfg_locate(c, c->text_ccf, LOC_TXT));

      /* Look for a project .ccf or a proxy project .ccf */
      const char *ccfpath;
      if ((ccfpath = cfy_cfg_locate(c, c->project_ccf, LOC_PRJ)))
	return cfy_cfg_load(c, ccfpath);
      else if ((ccfpath = cfy_cfg_locate(c, c->proxy_ccf, LOC_PRJ)))
	return cfy_cfg_load(c, ccfpath);
    }
  return 0;
}

int
cfy_cfg_text(Cfy *c)
{
  if (!c->arg_ccf)
    {
      if (c->protocol_ccf)
	return cfy_cfg_load(c, cfy_cfg_locate(c, c->protocol_ccf, LOC_TXT));
      else if (c->proxypro_ccf)
	return cfy_cfg_load(c, cfy_cfg_locate(c, c->proxypro_ccf, LOC_TXT));
    }
  return 0;
}

static int
cfy_cfg_rbox(Mloc m, Cfy *c, const char *unit, const char *colour, const char *style)
{
  curr_cp->rbox.e = ELT_Rb;
  if (unit)
    curr_cp->rbox.w = unit;
  if (colour)
    curr_cp->rbox.c = colour;
  if (colour)
    curr_cp->rbox.s = style;
  return 0;
}

static int
cfy_cfg_rline(Mloc m, Cfy *c, const char *unit, const char *colour, const char *style)
{
  curr_cp->rline.e = ELT_Rl;
  if (unit)
    curr_cp->rline.w = unit;
  if (colour)
    curr_cp->rline.c = colour;
  if (style)
    curr_cp->rline.s = style;
  return 0;
}

static int
cfy_cfg_rcol(Mloc m, Cfy *c, const char *unit, const char *colour, const char *style)
{
  curr_cp->rcol.e = ELT_Rc;
  if (unit)
    curr_cp->rcol.w = unit;
  if (colour)
    curr_cp->rcol.c = colour;
  if (style)
    curr_cp->rcol.s = style;
  return 0;
}

int
cfy_r(Mloc m, Cfy *c, Etype e, const char *unit, const char *colour, const char *style)
{
  if (unit || colour || style)
    ++curr_cp->ruledata;
  switch (e)
    {
    case ELT_Rb:
      return cfy_cfg_rbox(m, c, unit, colour, style);
      break;
    case ELT_Rl:
      return cfy_cfg_rline(m, c, unit, colour, style);
      break;
    case ELT_Rc:
      return cfy_cfg_rcol(m, c, unit, colour, style);
      break;
    default:
      /*can't happen*/
      fprintf(stderr, "cfy_r: bad Etype: %d\n", e);
      break;
    }
  return 0;
}

int
cfy_cfg_justify(Mloc m, Cfy *c, Etype e)
{
  curr_cp->justify = e;
  return 0;
}

void
cfy_cfg_font(Mloc m, Cfy *c, const char *pc, const char *nm)
{
  Class *cp = NULL;
  if (!strchr(nm,'-'))
    {
      const char *nmkey = cfy_class_key(nm, c->c->ffs, c->c->mag, c->c->scr, c->c->asl);
      cp = cfy_class(c, nmkey, c->c);
    }
  else
    {
      cp = cfy_class(c, nm, c->c);
    }
  c->fontclasses[atoi(pc)] = cp;
}

void
cfy_cfg_grid(Mloc m, Cfy *c, const char *t, const char *n)
{
  if ('o' == *t)
    curr_cp->grid.o = strcmp(n, "0") ? n : NULL;
  else if ('c' == *t)
     curr_cp->grid.c = strcmp(n, "0") ? n : NULL;
  else
    curr_cp->grid.s = strcmp(n, "0") ? n : NULL;
  ++c->need_style;
}

void
cfy_cfg_ui(Mloc m, Cfy *c, const char *u)
{
  if (!strcmp(u, "no-CuneifyBar"))
    c->no_CuneifyBar = 1;
}

int
cfy_cfg_key(Mloc m, Cfy *c, const char *k)
{
  curr_cp = c->c = cfy_class(c, k, c->c);
  return 0;
}

int
cfy_cfg_load(Cfy *c, const char *cfgpath)
{
  Cfg *cfgp = hash_find(c->hconfigs, (uccp)cfgpath);
  if (cfgp)
    {
      c->cfg = cfgp;
      return 0;
    }
  if (!access(cfgpath, R_OK))
    {
      FILE *cfp = fopen(cfgpath, "r");
      if (cfp)
	{
	  if (verbose)
	    fprintf(stderr, "cfy_cfg_load: using %s\n", cfgpath);
	  c->cfg = memo_new(c->m_cfg);
	  c->cfg->path = cfgpath;
 	  c->cfg->m_subspec = memo_init(sizeof(Subspec), 1024);
	  c->cfg->m_assignment = memo_init(sizeof(Assignment), 16);
	  hash_add(c->hconfigs, (uccp)c->cfg->path, c->cfg);
	  c->cfg->hsubhead = hash_create(128);
	  c->cfg->hsubkeys = hash_create(128);
	  gdl_init();
	  gvl_setup("osl", "osl", "020");
	  extern const char *curr_ccf;
	  extern FILE *cfyin;
	  curr_ccf = cfgpath;
	  cfyin = cfp;
	  cfyparse();
	  fclose(cfp);
	  gdl_term();

	  if (cfy_cfg_status)
	    {
	      extern int cfylineno;
	      mesg_warning(curr_ccf, cfylineno,
			   "cfy exiting because of configuration errors.");
	      mesg_print(stderr);
	      exit(1);
	    }
	  return 0;
	}
      else
	{
	  mesg_verr(NULL, "failed to open config file %s", cfgpath);
	  return 1;
	}
    }
  else
    {
      mesg_verr(NULL, "config file %s not readable", cfgpath);
      return 1;
    }
}

void
cfy_cfg_width(Mloc m, Cfy *c, const char *w)
{
  c->c->width = w;
}

void
cfy_cfg_col_unit(Mloc m, Cfy *c, const char *u)
{
  if (!cfy_col_units)
    cfy_col_units = list_create(LIST_SINGLE);
  list_add(cfy_col_units, (void*)u);
}

void
cfy_cfg_col_wrap(Mloc m, Cfy *c)
{
  c->c->colwidths = (ccp)list_join(cfy_col_units, ",");
  list_free(cfy_col_units, NULL);
  cfy_col_units = NULL;
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
    case ELT_S:
      return "s";
    case ELT_Hp:
      return "Hp";
    case ELT_Sp:
      return "Sp";
    case ELT_F:
      return "f";
    case ELT_R:
      return "r";
    case ELT_E:
      return "e";
    case ELT_X:
      return "x";
    case ELT_D:
      /* ignore deleted nodes */
      return NULL;
    case ELT_Q:
      return e->data;
    case ELT_Rb:
      return "Rb";
    case ELT_Rl:
      return "Rl";
    case ELT_Rc:
      return "Rc";
    case ELT_Jc:
      return "Jc";
    case ELT_Jcp:
      return "Jcp";
    case ELT_Jcs:
      return "Jcs";
    case ELT_Jl:
      return "Jl";
    case ELT_Jp:
      return "Jp";
    case ELT_Jr:
      return "Jr";
    case ELT_Js:
      return "Js";
    case ELT_A:
      /* only used in subbing rule rhs */
      return NULL;	  
    case ELT_LAST:
    default:
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
sp_clone(Cfy *cp, Subspec *sp)
{
  Cfg *c = cp->cfg;
  Subspec *clone = memo_new(c->m_subspec);
  *clone = *sp;
  return clone;
}

void
cfy_cfg_stash(Mloc m, Cfy *cp)
{
  Cfg *c = cp->cfg;
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
	  const char *ek = elts_key(cp, sp->l, sp->l_len);
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
		      hash_add(c->hsubkeys,
			       (uccp)hpool_copy((uccp)buf,cp->hp),
			       sp_clone(cp, sp));
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
  elts_rhs = 0;
}

static List *lhs = NULL, *rhs = NULL;

void
cfy_cfg_asgn(Mloc m, Cfy *cp, int nth, const char *memb, const char *val)
{
  Cfg *c = cp->cfg;
  if (elts_rhs)
    {
      Assignment *ap = memo_new(c->m_assignment);
      Elt *ep = memo_new(cp->m_elt);
      ep->etype = ELT_A;
      ep->data = ap;
      list_add(rhs, ep);
      sub_has_assignment = 1;
      if (nth > 0 && nth <= list_len(lhs))
	  ap->lindex = nth-1;
      else
	{
	  mesg_verr(&m, "@-index is should be > 0 and <= %d", list_len(lhs));
	  ++cfy_cfg_status;
	}
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
	  else
	    {
	      mesg_verr(&m, "%s is not a member of Elt", memb);
	      ++cfy_cfg_status;
	    }
	}
      else
	ap->offof = UINTPTR_MAX;
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

void
cfy_cfg_elt_s(Mloc m, Cfy *c)
{
  (void)cfy_cfg_elt(c, ELT_S);
}

void
cfy_cfg_elt_Jl(Mloc m, Cfy *c)
{
  (void)cfy_cfg_elt(c, ELT_Jl);
}

void
cfy_cfg_elt_Jp(Mloc m, Cfy *c)
{
  (void)cfy_cfg_elt(c, ELT_Jp);
}

void
cfy_cfg_elt_Jr(Mloc m, Cfy *c)
{
  (void)cfy_cfg_elt(c, ELT_Jr);
}

void
cfy_cfg_elt_Js(Mloc m, Cfy *c)
{
  (void)cfy_cfg_elt(c, ELT_Js);
}
