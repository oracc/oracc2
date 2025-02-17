#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include <hash.h>
#include <mesg.h>
#include <lng.h>
#include "cbd.h"
#include "cbd.tab.h"

extern int bang, star;
static int one = 1;
static List *curr_base_list = NULL;
struct parts *curr_parts;
List *cmt_queue = NULL;

Hash *cbds = NULL;

#define cmts(l) (l) = cmt_queue , cmt_queue = NULL;

struct alias *
cbd_bld_alias(YYLTYPE l, struct entry *e)
{
  struct alias *a = memo_new(e->owner->aliasmem);
  a->l = l;
  cmts(a->l.user);
  list_add(e->aliases, a);
  a->cgp = cgp_get_one();
  hash_add(e->owner->haliases, a->cgp->tight, e);
  return a;
}

void
cbd_bld_allow(YYLTYPE l, struct entry *e, unsigned char *lhs, unsigned char *rhs)
{
  struct allow *ap = NULL;
  Hash *h_a, *h_b;

  if (!e->b_allow)
    e->b_allow = hash_create(1024);
  if (!e->allows)
    e->allows = list_create(LIST_SINGLE);

  ap = memo_new(e->owner->allowmem);
  ap->lhs = lhs;
  ap->rhs = rhs;
  ap->l = l;
  cmts(ap->l.user);
  list_add(e->allows, ap);

  h_a = hash_find(e->b_allow, lhs);
  h_b = hash_find(e->b_allow, rhs);
  if (!h_a)
    {
      h_a = hash_create(1);
      hash_add(h_a, rhs, ap);
      hash_add(e->b_allow, lhs, h_a);
    }
  if (!h_b)
    {
      h_b = hash_create(1);
      hash_add(h_b, lhs, ap);
      hash_add(e->b_allow, rhs, h_b);
    }
}

/* At parse time we just save the bases in a list of lists */
void
cbd_bld_bases_pri(YYLTYPE l, struct entry *e, unsigned char *lang, unsigned char *p)
{
  if (p && strlen((ccp)p))
    {
      struct loctok *ltp = cbd_bld_loctok(&l,e,p);
      ltp->lang = (const char*)lang;
      if (!e->bases)
	e->bases = list_create(LIST_SINGLE);
      list_add(e->bases, (curr_base_list = list_create(LIST_SINGLE)));
      list_add(curr_base_list, ltp);
    }
  else
    {
      mesg_err(&l, "zero-length primary base (misplaced ';'?)");
    }
}

void
cbd_bld_bases_alt(YYLTYPE l, struct entry *e, unsigned char *a)
{
  if (a && strlen((ccp)a))
    {
      if (curr_base_list)
	list_add(curr_base_list, cbd_bld_loctok(&l,e,a));
    }
  else
    {
      mesg_err(&l, "zero-length alternate base (misplaced ','?)");
    }
}

struct cbd *
cbd_bld_cbd(void)
{
  struct cbd *c = NULL;
  extern const char *file; /*FIXME*/
  c = malloc(sizeof(struct cbd));
  c->aliasmem = memo_init(sizeof(struct alias), 1024);
  c->allowmem = memo_init(sizeof(struct allow), 1024);
  c->cgpmem = memo_init(sizeof(struct cgp), 1024);
  c->editmem = memo_init(sizeof(struct edit), 1024);
  c->equivmem = memo_init(sizeof(struct equiv), 1024);
  c->entrymem = memo_init(sizeof(struct entry), 1024);
  c->formsmem = memo_init(sizeof(Form), 1024);
  c->i18nmem = memo_init(sizeof(struct i18n), 1024);
  c->metamem = memo_init(sizeof(struct meta), 1024);
  c->locatormem = memo_init(sizeof(locator), 1024);
  c->loctokmem = memo_init(sizeof(struct loctok), 1024);
  c->metaordermem = memo_init(sizeof(struct metaorder), 1024);
  c->partsmem = memo_init(sizeof(struct parts), 1024);
  c->pleiadesmem = memo_init(sizeof(struct pleiades), 1024);
  c->sensesmem = memo_init(sizeof(struct sense), 1024);
  c->tagmem = memo_init(sizeof(struct tag), 1024);
  c->taglmem = memo_init(sizeof(struct tagl), 1024);
  c->pool = pool_init();
  c->letters = list_create(LIST_SINGLE);
  c->entries = list_create(LIST_SINGLE);
  c->edits = list_create(LIST_SINGLE);
  c->propss = list_create(LIST_SINGLE);
  c->reldefs = list_create(LIST_SINGLE);
  c->haliases = hash_create(16);
  c->hentries = hash_create(1024);
  c->l.file = (char *)file;
  c->l.line = 1;
  return c;
}

void
cbd_bld_cbd_setup(struct cbd*c)
{
  c->iname = malloc(strlen((ccp)c->project) + strlen((ccp)c->lang) + 2);
  sprintf((char*)c->iname, "%s:%s", c->project, c->lang);
  hash_add(cbds, c->iname, c);
  /* xpdinit */
  /* cuneify_init(c->xpd); */
}

void
cbd_bld_cbd_term(struct cbd*c)
{
  pool_term(c->pool);
  hash_add(cbds, c->iname, NULL);
  /*cuneify_term();*/
  free(c);
}

List *
cbd_bld_cmt_append(List *to, List *from)
{
  if (!to)
    to = from;
  else if (to == from)
    return to;
  else
    {
      List_node *lp;
      for (lp = from->first; lp; lp = lp->next)
	list_add(to, lp->data);
      list_free(from, NULL);
    }
  return to;
}

void
cbd_bld_cmt_queue(locator *lp, unsigned char *cmt)
{
  if (curr_entry)
    {
      if (!cmt_queue)
	cmt_queue = list_create(LIST_SINGLE);
      list_add(cmt_queue, cmt);
    }
  else
    mesg_err(lp, "#comments are only allowed inside entries");
}

void
cbd_bld_dcf(YYLTYPE l, struct entry *e, unsigned char *dcf, unsigned char *dcfarg)
{
  struct tag *tp;
  if (!e->dcfs)
    e->dcfs = list_create(LIST_SINGLE);
  if (!e->hdcfs)
    e->hdcfs = hash_create(1024);
  tp = memo_new(e->owner->tagmem);
  tp->l = l;
  cmts(tp->l.user);
  tp->name = (ccp)dcf;
  tp->val = dcfarg;
  if (tp->val[0] == '=' && !tp->val[1])
    {
      struct cbdrws *cp = cbdrws(tp->name, strlen(tp->name));
      if (cp)
	e->lang = (const char *)cp->lang;
    }
  list_add(e->dcfs, dcf);
  hash_add(e->hdcfs, dcf, tp);
}

void
cbd_bld_discl(YYLTYPE l, struct entry *e, const char *lang, unsigned char *text, int e_or_s)
{
  if (e_or_s) /* apply to sense */
    {
      struct sense *sp = list_last(e->senses);
      struct sense *senselp = NULL;
      List_node *tmp;
      for (tmp = sp->sensels->first; tmp; tmp = tmp->next)
	{
	  if (!strcmp(lang, (const char*)((struct sense *)(tmp->data))->lng))
	    {
	      senselp = tmp->data;
	      break;
	    }
	}
      if (senselp)
	senselp->disc = cbd_bld_tag(l, e, "discl", text);
      else
	fprintf(stderr, "lang %s not found in sensels\n", lang);
    }
  else
    {
      struct i18n *i18 = memo_new(e->owner->i18nmem);
      i18->l = l;
      cmts(i18->l.user);
      i18->lang = lang;
      i18->data = text;
      if (!e->discls)
	e->discls = list_create(LIST_SINGLE);
      list_add(e->discls, i18);
    }
}

void
cbd_bld_edit(struct entry *e, char ctxt, char type)
{
  struct edit *ed = memo_new(e->owner->editmem);
  /*struct sense *snode = NULL;*/
  if (ctxt == 's')
    {
      struct sense *sp = list_last(e->senses);
      ed->owner = sp;
      sp->ed = ed;
      ed->lp = &sp->l;
    }
  else
    {
      ed->owner = e;
      e->ed = ed;
      ed->lp = &e->l;
    }
  switch (type)
    {
    case '+':
      ed->type = (ctxt == 'e' ? ADD_E : ADD_S);
      break;
    case '-':
      ed->type = (ctxt == 'e' ? DEL_E : DEL_S);
      break;
    case '>':
      ed->type = (ctxt == 'e' ? REN_E : REN_S);
      ed->target = cgp_get_one();
      break;
    case '|':
      ed->type = (ctxt == 'e' ? MRG_E : MRG_S);
      ed->target = cgp_get_one();
      break;
    default:
      break;
    }
}

void
cbd_bld_edit_entry(struct entry *e, char type)
{
  cbd_bld_edit(e, 'e', type);
}

struct sense *
cbd_bld_edit_sense(struct entry *e, char type)
{
  cbd_bld_edit(e, 's', type);
  return ((struct sense *)(list_last(e->senses)))->ed->sp = memo_new(e->owner->sensesmem);
}

void
cbd_bld_edit_why(struct entry *e, char *why)
{
  if (e->ed)
    e->ed->why = (ucp)why;
  /* should error if #why: doesn't follow an edit and valid it's only after -@entry */
}

struct entry *
cbd_bld_entry(YYLTYPE l, struct cbd* c)
{
  struct entry *e = memo_new(c->entrymem);
  e->aliases = list_create(LIST_SINGLE);
  e->forms = list_create(LIST_SINGLE);
  e->senses = list_create(LIST_SINGLE);
  e->owner = c;
  e->lang = (const char *)c->lang;
  list_add(c->entries, e);
  e->meta = memo_new(c->metamem);
  e->l = l;
  cmts(e->l.user);
  if (bang)
    {
      bang = 0;
      e->bang = 1;
    }
  if (star)
    {
      star = 0;
      e->usage = 1;
    }
  return e;
}

void
cbd_bld_entry_cgp(struct entry *e)
{
  e->cgp = cgp_get_one();
  hash_add(e->owner->hentries, e->cgp->tight, &one);
}

#if 0
static void
cbd_bld_entry_term(struct entry *e)
{
  if (e->b_pri)
    {
      hash_free(e->b_pri, NULL);
      hash_free(e->b_alt, NULL);
      hash_free(e->b_sig, NULL);
      e->b_pri = e->b_alt = e->b_sig = e->b_allow = NULL;
    }
  if (e->b_allow)
    {
      hash_free(e->b_allow, NULL);
      e->b_allow = NULL;
    }
}
#endif

struct equiv *
cbd_bld_equiv(struct entry *e, unsigned char *lang, unsigned char *text)
{
  struct equiv *eq = memo_new(e->owner->equivmem);
  eq->lang = lang;
  eq->text = text;
  return eq;
}

Form *
cbd_bld_form(YYLTYPE l, struct entry *e)
{
  static Form *f2p;
  f2p = memo_new(e->owner->formsmem);
  f2p->file = (ucp)l.file;
  f2p->lnum = l.line;
  cmts(f2p->user);
  list_add(e->forms, f2p);
  if (bang)
    {
      bang = 0;
      f2p->rank = 1; /* check this is the right value for form! rank */
    }
  return f2p;
}

void
cbd_bld_form_setup(struct entry *e, Form* f2p)
{
  f2p->project = e->owner->project;
  if (!f2p->lang)
    f2p->lang = (ucp)e->lang;
  f2p->core = langcore_of((ccp)f2p->lang);
  f2p->cf = e->cgp->cf;
  f2p->gw = e->cgp->gw;
  f2p->pos = e->cgp->pos;
}

void
cbd_bld_gwl(YYLTYPE l, struct entry *e, const char *lang, unsigned char *text)
{
  struct i18n *i18 = memo_new(e->owner->i18nmem);
  i18->l = l;
  cmts(i18->l.user);
  i18->lang = lang;
  i18->data = text;
  if (!e->gwls)
    e->gwls = list_create(LIST_SINGLE);
  list_add(e->gwls, i18);
}

locator *
cbd_bld_locator(YYLTYPE l)
{
  locator *lp = memo_new(curr_cbd->locatormem);
  *lp = l;
  return lp;
}

struct loctok *
cbd_bld_loctok(YYLTYPE *lp, struct entry *e, unsigned char *tok)
{
  struct loctok *ltp = memo_new(e->owner->loctokmem);
  ltp->l = *lp;
  cmts(ltp->l.user);
  ltp->tok = tok;
  return ltp;
}

struct meta *
cbd_bld_meta_create(struct entry *e)
{
  return memo_new(e->owner->metamem);
}

void
cbd_bld_meta_add(YYLTYPE l, struct entry *e, struct meta *mp, int tok, const char *name, void *val)
{
  List *lp = NULL;
  struct metaorder *orderp;
  switch (tok)
    {
    case BIB:
      if (!mp->bib)
	mp->bib = list_create(LIST_SINGLE);
      lp = mp->bib;
      break;
    case COLLO:
      if (!mp->collo)
	mp->collo = list_create(LIST_SINGLE);
      lp = mp->collo;
      break;
    case EQUIV:
      if (!mp->equiv)
	mp->equiv = list_create(LIST_SINGLE);
      lp = mp->equiv;
      break;
    case INOTE:
      if (!mp->inote)
	mp->inote = list_create(LIST_SINGLE);
      lp = mp->inote;
      break;
    case ISSLP:
      if (!mp->isslp)
	mp->isslp = list_create(LIST_SINGLE);
      lp = mp->isslp;
      break;
    case NOTE:
      if (!mp->note)
	mp->note = list_create(LIST_SINGLE);
      lp = mp->note;
      break;
    case OID:
      if (!mp->oid)
	mp->oid = list_create(LIST_SINGLE);
      lp = mp->oid;
      break;
    case PROP:
      if (!mp->prop)
	mp->prop = list_create(LIST_SINGLE);
      lp = mp->prop;
      break;
    case PL_COORD:
      if (!mp->pleiades)
	mp->pleiades = list_create(LIST_SINGLE);
      lp = mp->pleiades;
      break;
#if 0
    case REL:
      lp = mp->rel;
      break;
#endif
    default:
      fprintf(stderr, "meta_add: unhandled token %d\n", tok);
      break;
    }
  if (lp)
    {
      list_add(lp, val);
      orderp = memo_new(e->owner->metaordermem);
      orderp->l = l;
      cmts(orderp->l.user);
      orderp->name = name;
      orderp->tok = tok;
      orderp->val = val;
      if (!mp->order)
	mp->order = list_create(LIST_SINGLE);
      list_add(mp->order, orderp);
    }
}

void
cbd_bld_note(YYLTYPE l, struct entry *e, struct meta *curr_meta, unsigned char *text)
{
  struct tagl *tlp = memo_new(e->owner->taglmem);
  tlp->l = l;
  cmts(tlp->l.user);
  tlp->data = text;
  if (!curr_meta->note)
    curr_meta->note = list_create(LIST_SINGLE);
  list_add(curr_meta->note, tlp);
}

void
cbd_bld_notel(YYLTYPE l, struct entry *e, struct meta *curr_meta, const char *lang, unsigned char *text)
{
  struct i18n *i18p = memo_new(e->owner->i18nmem);
  struct tagl *tp = list_last(curr_meta->note);
  i18p->l = l;
  cmts(i18p->l.user);
  i18p->data = text;
  list_add(tp->i18n, i18p);
}

struct parts *
cbd_bld_parts(YYLTYPE l, struct entry *e)
{
  struct parts *pp = memo_new(e->owner->partsmem);
  if (!e->parts)
    e->parts = list_create(LIST_SINGLE);
  list_add(e->parts, pp);
  pp->l = l;
  cmts(pp->l.user);
  return pp;
}

struct pleiades *
cbd_bld_pl_id(YYLTYPE l, struct entry *e, unsigned char *id)
{
  struct pleiades *p = memo_new(e->owner->pleiadesmem);
  p->l_id = l;
  cmts(p->l_id.user);
  p->id = id;
  return p;
}

void
cbd_bld_pl_coord(YYLTYPE l, struct pleiades *p, unsigned char *coord)
{
  p->l_coord = l;
  cmts(p->l_coord.user);
  p->coord = coord;
}

void
cbd_bld_pl_alias(YYLTYPE l, struct pleiades *p, const char *lang, unsigned char *alias)
{
  struct loctok *ltp = memo_new(curr_cbd->loctokmem);
  if (!p->pl_aliases)
    p->pl_aliases = list_create(LIST_SINGLE);
  list_add(p->pl_aliases, ltp);
  ltp->l = l;
  cmts(ltp->l.user);
  ltp->lang = (ccp)lang;
  ltp->tok = alias;
}

void
cbd_bld_props(struct cbd *c, char *text)
{
  if (!c->propss)
    c->propss = list_create(LIST_SINGLE);
  list_add(c->propss, text);
}

void
cbd_bld_reldef(struct cbd *c, char *text)
{
  if (!c->reldefs)
    c->reldefs = list_create(LIST_SINGLE);
  list_add(c->reldefs, text);
}

struct sense *
cbd_bld_sense(YYLTYPE l, struct entry *e)
{
  struct sense *sp = memo_new(e->owner->sensesmem);
  sp->l = l;
  cmts(sp->l.user);
  list_add(e->senses, sp);
  if (bang)
    {
      bang = 0;
      sp->bang = 1;
    }
  return sp;
}

struct sense *
cbd_bld_sensel(YYLTYPE l, struct entry *e)
{
  struct sense *sp = memo_new(e->owner->sensesmem);
  struct sense *curr_sense = list_last(e->senses);
  sp->l = l;
  cmts(sp->l.user);
  list_add(curr_sense->sensels, sp);
  return sp;
}

void
cbd_bld_stem(YYLTYPE l, struct entry *e, unsigned char *stem)
{
  if (!e->stems)
    e->stems = list_create(LIST_SINGLE);
  
  list_add(e->stems, cbd_bld_loctok(&l,e,stem));
}

struct tag *
cbd_bld_tag(YYLTYPE l, struct entry *e, const char *name, unsigned char *val)
{
  struct tag *tp;
  tp = memo_new(e->owner->tagmem);
  tp->l = l;
  cmts(tp->l.user);
  tp->name = name;
  tp->val = val;
  return tp;
}

