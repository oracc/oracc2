#include <stdio.h>
#include <rnvif.h>
#include <xmlify.h>
#include <pool.h>
#include <joxer.h>
#include <rnvxml.h>
#include <gt.h>
#include "gx.h"
#include "o_jox_fncs.c"

locator *xo_loc;
extern void iterator(struct cbd *c, iterator_fnc fncs[]);
static void o_jox_proplist(const char *p);

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
o_jox_bases(struct entry *e)
{
#if 0
  List_node *outer;

  xo_loc = &((struct loctok *)((List*)(e->bases->first->data))->first->data)->l;
  joxer_eaaa(xo_loc,"bases", NULL);
  for (outer = e->bases->first; outer; outer = outer->next)
    {
      List *bp = ((List *)(outer->data));
      List_node *inner = bp->first;
      struct loctok *blt = NULL;

      blt = inner->data;
      xo_loc = &((struct loctok *)inner->data)->l;
      joxer_ea(xo_loc,"base", NULL);

      if (!blt->lang)
	blt->lang = e->lang;
      joxer_ea(xo_loc,"pri", rnvval_aa("av", "xml:lang", blt->lang, (ccp)NULL));
      joxer_ch(xo_loc,(const char *)blt->tok);
      joxer_ee(xo_loc,"pri");
      if (list_len(bp) > 1)
	{
	  for (inner = inner->next; inner; inner = inner->next)
	    {
	      joxer_ea(xo_loc,"alt", NULL);
	      joxer_ch(xo_loc,(const char *)((struct loctok *)inner->data)->tok);
	      joxer_ee(xo_loc,"alt");
	    }
	}
      joxer_ee(xo_loc,"base");
    }
  joxer_eeaa(xo_loc,"bases");
  #endif
}

static void
o_jox_cbd(struct cbd *c)
{
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
    hash_add(h, (uccp)f[i]->k[1], f[i]);
  return h;
}

static void
o_jox_field(Entry *e, Efield ef, Field **f, const char *tag)
{
  int i;
  char tagses[strlen(tag)+2];
  sprintf(tagses, "%ss", tag);

  Hash *hnmfm = NULL;
  if (EFLD_NORM == ef)
    hnmfm = nmfm_hash(e->hshary[EFLD_NMFM]);
  
  joxer_ea(xo_loc, tagses, NULL);
  for (i = 0; f[i]; ++i)
    {
      /* f[i]->data is NULL when processing periods as field data */
      if (f[i]->data)
	{
	  switch (ef)
	    {
	    case EFLD_FORM:
	    case EFLD_BASE:
	      {
		joxer_ea(xo_loc, tag, ratts_form(f[i], O_XML));
		Gt *t = ((Cform*)f[i]->data)->t;
		if (t)
		  {
		    joxer_et(xo_loc, "s", NULL, (ccp)((Cform*)f[i]->data)->f.form);
		    joxer_ea(xo_loc,"t", NULL);
		    grx_jox(t->gdl, "g:w");
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
o_jox_forms(struct entry *e)
{
  if (e->hshary[EFLD_FORM])
    o_jox_field(e, EFLD_FORM, e->hshary[EFLD_FORM], "form");
  if (e->hshary[EFLD_NORM])
    o_jox_field(e, EFLD_NORM, e->hshary[EFLD_NORM], "norm");
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
o_jox_parts(struct entry *e)
{
#if 0
  List_node *lp;
  for (lp = e->parts->first; lp; lp = lp->next)
    {
      struct parts *p = (struct parts*)lp->data;
      printf("<partss>");
      if (p->cgps && list_len(p->cgps))
	{
	  printf("<parts>");
	  List_node *cp;
	  for (cp = p->cgps->first; cp; cp = cp->next)
	    printf("<cgp>%s</cgp>", ((struct cgp*)(cp->data))->tight);
	  printf("</parts>");
	}
      printf("</partss>");
    }
#endif
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
#if 0
  if (e->begin_senses)
    f1(/* @senses */ );

  List_node *lp;
  for (lp = e->senses->first; lp; lp = lp->next)
    {
      struct sense *sp = (struct sense*)(lp->data);

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
      
      f0(/* @sense */ );
      if (sp->lng)
	{ f1(/* % */ sp->lng); }
      if (sp->sid)
	f1(/* # */ sp->sid);
      if (sp->num)
	f1(/* . */ sp->num);
      if (sp->sgw)
	f1(/* [%s] */ sp->sgw);
      if (sp->pos)
	f1(/*  */ sp->pos);
      if (sp->mng)
	f1(/*  */ sp->mng);

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
      if (sp->disc)
	f1(/* @disc */ sp->disc);
    }
  
  if (e->begin_senses)
    f0(/* @end senses */ );
#endif
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
