#include <stdio.h>
#include <rnvif.h>
#include <xmlify.h>
#include <pool.h>
#include <joxer.h>
#include "gx.h"
#include "o_jsn_fncs.c"
#include "rnvxml.h"

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
o_jsn(struct cbd*cbd)
{
  xo_loc = malloc(sizeof(locator));
  xo_loc->file = file = cbd->file;
  xo_loc->line = 1;
  iterator_fnc *fncs = ifnc_init();
  iterator(cbd,fncs);
  free(fncs);
}

static void
o_jsn_aliases(struct entry *e)
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
o_jsn_allow(struct entry *e)
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
o_jsn_bases(struct entry *e)
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
o_jsn_cbd(struct cbd *c)
{
  struct rnvval_atts *ratts = rnvval_aa("cbd",
					"type", "cbd",
					"project", c->project,
					"xml:lang", c->lang,
					"name", c->name,
					(ccp)NULL);
#if 1
  jsn_xmlns_atts = NULL;
  joxer_ea(xo_loc, "cbd", ratts);
  joxer_ao("entries");
#else
  joxer_ea(xo_loc, "cbd", ratts);
#endif
}

static void
o_jsn_dcfs(struct entry *e)
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
o_jsn_entry(struct entry *e)
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
  ( /* @entry */ e->cgp->loose);
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
  joxer_ea(xo_loc,"entry", NULL);
  if (e->disc)
    f1(/* @disc */ e->disc);
}

static void
o_jsn_end_cbd(struct cbd *c)
{
  joxer_ac();
  joxer_ee(xo_loc,"cbd");
}

static void
o_jsn_end_entry(struct entry *e)
{
  joxer_ee(xo_loc,"entry");
}

static void
o_jsn_forms(struct entry *e)
{
#if 0
  if (e->forms && list_len(e->forms))
    {
      List_node *lp;
      for (lp = e->forms->first; lp; lp = lp->next)
	{
	  Form *f2p = (Form*)(lp->data);
	  f1(/* @form */ f2p->form);
	  if (f2p->lang) /* careful: we only should only emit this if lang is explicit in form */
	    f1(/* %% */ f2p->lang);
	  if (f2p->base)
	    f1(/* /%s */ f2p->base);
	  if (f2p->stem)
	    f1(/* * */ f2p->stem);
	  if (f2p->cont)
	    f1(/* + */ f2p->cont);
	  if (f2p->morph)
	    f1(/* # */ f2p->morph);
	  if (f2p->morph2)
	    f1(/* ## */ f2p->morph2);
	  if (f2p->norm)
	    f1(/* $ */ f2p->norm);
	}
    }
#endif
}

static void
o_jsn_meta(struct entry *e)
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
o_jsn_parts(struct entry *e)
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
o_jsn_phon(struct entry *e)
{
  f1(/* @phon */ (ccp)e->phon);
}

static void
o_jsn_proplist(const char *p)
{
  f1(/* @proplist */ p);
}

static void
o_jsn_root(struct entry *e)
{
  f1(/* @root */ (ccp)e->root);
}

static void
o_jsn_senses(struct entry *e)
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
o_jsn_stems(struct entry *e)
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
