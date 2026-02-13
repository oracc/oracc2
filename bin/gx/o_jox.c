#include <stdio.h>
#include <rnvif.h>
#include <xmlify.h>
#include <pool.h>
#include <joxer.h>
#include <rnvxml.h>
#include <gt.h>
#include "gx.h"
#include "o_jox_fncs.c"

static int sense_context;

locator *xo_loc;
extern void iterator(struct cbd *c, iterator_fnc fncs[]);
static void o_jox_proplist(const char *p);
static void o_jox_field(void *e, Efield ef, Field **f, const char *tag);

#define f0()
#define f1(a)
#define f2(a,b)
#define f3(a,b,c)

#define CBD2NS "http://oracc.org/ns/cbd/2.0"

static const char *cbd2ns = CBD2NS;
static Hash *cbd_qnames = NULL;
static Hash *cbd_qanames = NULL;

void
o_jox(struct cbd*cbd)
{
  xo_loc = malloc(sizeof(locator));
  xo_loc->file = file = cbd->file;
  xo_loc->line = 1;
  iterator_fnc *fncs = ifnc_init();
  iterator(cbd,fncs);
  free(fncs);
}

static void
o_jox_aliases(struct entry *e)
{
#if 0
  List_node *lp;
  xo_loc = &((struct alias *)e->aliases->first->data)->l;
  joxer_ea(xo_loc,"aliases", NULL);
  for (lp = e->aliases->first; lp; lp = lp->next)
    {
      struct alias *ap = lp->data;
      xo_loc = &ap->l;
      joxer_ea(xo_loc,"alias", NULL);
      joxer_ch(xo_loc,(const char*)ap->cgp->tight);
      joxer_ee(xo_loc,"alias");
    }
  joxer_ee(xo_loc,"aliases");
#endif
}

static void
o_jox_allow(struct entry *e)
{
#if 0
  List_node *lp;
  xo_loc = &((struct alias *)e->allows->first->data)->l;
  joxer_ea(xo_loc,"allows", NULL);
  for (lp = e->allows->first; lp; lp = lp->next)
    {
      struct allow *ap = lp->data;
      xo_loc = &ap->l;
      struct rnvval_atts *ratts = rnvval_aa("va",
					    "lhs", ap->lhs,
					    "rhs", ap->rhs,
					    (ccp)NULL);
      joxer_ec(xo_loc,"allow",ratts);
    }
  joxer_ee(xo_loc,"allows");
#endif
}

static void
o_jox_bases(void *e)
{
  if (sense_context)
    {
      if (((Sense*)e)->hshary[EFLD_BASE])
	o_jox_field(e, EFLD_BASE, ((Sense*)e)->hshary[EFLD_BASE], "base");
    }
  else
    {
      if (((Entry*)e)->hshary[EFLD_BASE])
	o_jox_field(e, EFLD_BASE, ((Entry*)e)->hshary[EFLD_BASE], "base");
    }
}

static void
o_jox_cbd(struct cbd *c)
{
  xo_loc = &c->l;
  Ratts *ratts = ratts_cbd(c, O_XML);
  joxer_ea(xo_loc, "entries", ratts);
}

static void
o_jox_dcfs(struct entry *e)
{
#if 0
  List_node *lp;
  for (lp = e->dcfs->first; lp; lp = lp->next)
    {
      unsigned char *lhs = ((ucp)(lp->data));
      unsigned char *rhs = hash_find(e->hdcfs, lhs);
      f2(lhs, rhs);
    }
#endif
}

static void
o_jox_entry(struct entry *e)
{
  xo_loc = &e->l;
  if (e->ed)
    {
      switch (e->ed->type)
	{
	case ADD_E:
	  /* + */;
	  break;
	case DEL_E:
	  /* - */;
	  break;
	default:
	  break;
	}
    }
  if (e->ed)
    {
      switch (e->ed->type)
	{
	case DEL_E:
	  f1(/* #why */ e->ed->why);
	  break;
	case REN_E:
	  f1(/* > */ e->ed->target->tight);
	  break;
	case MRG_E:
	  f1(/* >> */ e->ed->target->tight);
	  break;
	default:
	  break;
	}
    }
  Ratts*ratts = ratts_entry(e, O_XML);
  joxer_ea(xo_loc, "entry", ratts);
  joxer_et(xo_loc, "cf", NULL, (ccp)e->cgp->cf);
  ratts = ratts_one("xml:lang", e->owner->trans ? (ccp)e->owner->trans : "en");
  joxer_et(xo_loc, "gw", ratts, (ccp)e->cgp->gw);
  joxer_et(xo_loc, "pos", NULL, (ccp)e->cgp->pos);
  if (e->disc)
    f1(/* @disc */ e->disc);
}

static void
o_jox_end_cbd(struct cbd *c)
{
  joxer_ee(xo_loc,"entries");
}

static void
o_jox_end_entry(struct entry *e)
{
  joxer_ee(xo_loc,"entry");
}

/* This function is passed a pointer to a NORM field; the user member
 * has an array of NmFm* with precomputed norm-form keys.
 */
static void
o_jox_nmfms(Field *f, Hash *hnmfm)
{
  NmFm **nfp = f->user;
  int i;
  joxer_ea(xo_loc,"forms", NULL);
  for (i = 0; nfp[i]; ++i)
    {
      Field *nmfmf = hash_find(hnmfm, (uccp)nfp[i]->nmfmk);
      if (nmfmf)
	{
	  List *lp = list_create(LIST_SINGLE);
	  list_pair(lp, "n", nfp[i]->form->f.form);
	  list_pair(lp, "cbd:id", f->id);
	  list_pair(lp, "ref", nfp[i]->form->f.user);
	  if (nmfmf->k[0])
	    ratts_kis(lp, nmfmf->k);
	  Ratts *ratts = ratts_list2ratts(lp);
	  joxer_ec(xo_loc, "f", ratts);
	}
      else
	fprintf(stderr, "o_jox_nmfms: internal error: norm-form key %s not found\n", nfp[i]->nmfmk);
    }
  joxer_ee(xo_loc,"forms");
}

/* Create a hash of norm-form keys to norm-form Fields */
static Hash *
nmfm_hash(Field **f)
{
  Hash *h = hash_create(10);
  int i;
  for (i = 0; f[i]; ++i)
    {
      fprintf(stderr, "nmfm_hash: adding %s\n", f[i]->k[1]);
      hash_add(h, (uccp)f[i]->k[1], f[i]);
    }
  return h;
}

static void
o_jox_field(void *e, Efield ef, Field **f, const char *tag)
{
  if (!f || !f[0])
    return;
  
  xo_loc = &((Cform*)f[0]->data)->l;
  int i;
  char tagses[strlen(tag)+2];
  sprintf(tagses, "%ss", tag);

  Hash *hnmfm = NULL;
  if (EFLD_NORM == ef)
    hnmfm = nmfm_hash(sense_context ? ((Sense*)e)->hshary[EFLD_NMFM] : ((Entry*)e)->hshary[EFLD_NMFM]);

  joxer_ea(xo_loc, tagses, NULL);
  for (i = 0; f[i]; ++i)
    {
      xo_loc = &((Cform*)f[i]->data)->l;
      /* f[i]->data is NULL when processing periods as field data */
      if (f[i]->data)
	{
	  switch (ef)
	    {
	    case EFLD_FORM:
	    case EFLD_BASE:
	      {
		joxer_ea(xo_loc, tag,
			 ef==EFLD_FORM ? ratts_form(f[i], O_XML) : ratts_base(f[i], O_XML));
		Gt *t = ef==EFLD_FORM ? ((Cform*)f[i]->data)->t : ((Cform*)f[i]->data)->b;
		if (t)
		  {
		    joxer_et(xo_loc,"s", NULL, (ccp)t->sign);
		    joxer_ea(xo_loc,"t", NULL);
		    grx_jox(t->gdl, NULL);
		    joxer_ee(xo_loc,"t");
		  }
		joxer_ee(xo_loc,tag);
	      }
	      break;
	    case EFLD_NORM:
	      {
		if (((Cform*)f[i]->data)->f.norm)
		  {
		    joxer_ea(xo_loc, tag, ratts_norm(f[i], O_XML));
		    joxer_et(xo_loc, "n", NULL, (ccp)((Cform*)f[i]->data)->f.norm);
		    if (f[i]->user)
		      o_jox_nmfms(f[i], hnmfm);
		    joxer_ee(xo_loc, tag);
		  }
	      }
	      break;
	    default:
	      break;
	    }	  
	}
    }
  joxer_ee(xo_loc, tagses);
  if (hnmfm)
    hash_free(hnmfm, NULL);
}

static void
o_jox_forms(void *e)
{
  if (sense_context)
    {
      if (((Sense*)e)->hshary[EFLD_FORM])
	o_jox_field(e, EFLD_FORM, ((Sense*)e)->hshary[EFLD_FORM], "form");
      if (((Sense*)e)->hshary[EFLD_NORM])
	o_jox_field(e, EFLD_NORM, ((Sense*)e)->hshary[EFLD_NORM], "norm");
    }
  else
    {
      if (((Entry*)e)->hshary[EFLD_FORM])
	o_jox_field(e, EFLD_FORM, ((Entry*)e)->hshary[EFLD_FORM], "form");
      if (((Entry*)e)->hshary[EFLD_NORM])
	o_jox_field(e, EFLD_NORM, ((Entry*)e)->hshary[EFLD_NORM], "norm");
    }
}

static void
o_jox_meta(struct entry *e)
{
#if 0
  if (e->meta && e->meta->order)
    {
      List_node *lp;
      for (lp = e->meta->order->first; lp; lp = lp->next)
	{
	  struct metaorder *mo = ((struct metaorder*)(lp->data));
	  const char *at = NULL;
	  
	  switch (mo->tok)
	    {
	    case BIB:
	      at = "bib";
	      break;
	    case COLLO:
	      at = "collo";
	      break;
	    case INOTE:
	      at = "inote";
	      break;
	    case ISSLP:
	      at = "isslp";
	      break;
	    case NOTE:
	      at = "note";
	      break;
	    case OID:
	      at = "oid";
	      break;
	    case PROP:
	      at = "prop";	      
	      break;
	    case EQUIV:
	      {
		struct equiv *val = (struct equiv *)(mo->val);
		f2(/* @equiv % */ val->lang, val->text);
	      }
	      break;
	    case PL_ID:
	      {
		struct pleiades *val = (struct pleiades*)(mo->val);
		f3(/* @pl_coord @pl_id @pl_uid */ val->coord, val->id, val->uid);
	      }
	      break;
#if 0
	    case REL:
	      lp = mp->rel;
	      break;
#endif
	    default:
	      fprintf(stderr, "unhandled tok %d\n", mo->tok);
	      break;
	    }
	  if (at)
	    f1(/* at */ (const char*)mo->val);
	}
    }
#endif
}

static void
o_jox_parts(Entry *e)
{
  List_node *lp;
  for (lp = e->parts->first; lp; lp = lp->next)
    {
      Parts *p = (Parts*)lp->data;
      joxer_ea(xo_loc,"compound",NULL);
      if (p->cgps && list_len(p->cgps))
	{
	  Cgp *cp;
	  for (cp = list_first(p->cgps); cp; cp = list_next(p->cgps))
	    {
	      Ratts *r = ratts_cpd(cp, O_XML);
	      joxer_ea(xo_loc, "cpd", r);
	      joxer_et(xo_loc, "cf", NULL, (ccp)cp->cf);
	      joxer_et(xo_loc, "gw", NULL, (ccp)cp->gw);
	      if (cp->mng)
		joxer_et(xo_loc, "mng", NULL, (ccp)cp->mng);
	      joxer_et(xo_loc, "pos", NULL, (ccp)cp->pos);
	      if (cp->epos)
		joxer_et(xo_loc, "epos", NULL, (ccp)cp->epos);
	      joxer_ee(xo_loc,"cpd");
	    }
	}
      joxer_ee(xo_loc,"compound");
    }
}

static void
o_jox_phon(struct entry *e)
{
  f1(/* @phon */ (ccp)e->phon);
}

static void
o_jox_proplist(const char *p)
{
  f1(/* @proplist */ p);
}

static void
o_jox_root(struct entry *e)
{
  f1(/* @root */ (ccp)e->root);
}

static void
o_jox_senses(struct entry *e)
{
  sense_context = 1;

  joxer_ea(xo_loc,"senses",NULL);

  Sense *sp;
  for (sp = list_first(e->senses); sp; sp = list_next(e->senses))
    {
#if 0
      if (sp->ed)
	{
	  switch (sp->ed->type)
	    {
	    case ADD_S:
	      putchar('+');
	      break;
	    case DEL_S:
	      putchar('-');
	      break;
	    default:
	      break;
	    }
	}
#endif

      Ratts *r = ratts_sense(sp, O_XML);
      joxer_ea(xo_loc,"sense",r);
      joxer_et(xo_loc, "pos", NULL, (ccp)sp->pos);
      joxer_et(xo_loc, "mng", NULL, (ccp)sp->mng);

      o_jox_bases(sp);

      o_jox_forms(sp);

      joxer_ee(xo_loc,"sense");      
#if 0
      if (sp->ed)
	{
	  switch (sp->ed->type)
	    {
	    case REN_S:
	      f2(/* > %s */ sp->ed->sp->pos, sp->ed->sp->mng);
	      break;
	    case MRG_S:
	      f2(/* >> %s */ sp->ed->sp->pos, sp->ed->sp->mng);
	      break;
	    default:
	      break;
	    }
	}
#endif

      if (sp->disc)
	f1(/* @disc */ sp->disc);
    }
  joxer_ee(xo_loc,"senses");
  sense_context = 0;
}

static void
o_jox_stems(struct entry *e)
{
#if 0
  List_node *lp;
  f0(/* @stems */ );
  for (lp = e->stems->first; lp; lp = lp->next)
    {
      unsigned char *stem = /* (ucp)(lp->data) */"";
      f1(stem);
    }
#endif
}
