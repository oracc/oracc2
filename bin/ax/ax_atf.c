#include <oraccsys.h>
#include <atf.h>
#include "ax.h"

FILE *axa_fp;

typedef void (*ltfnc)(Mloc *mp, void *p);

static void
axa_andline(Mloc *mp, ATF *p)
{
  axa_fp = stderr;
  fprintf(axa_fp, "&%s = %s\n", p->pqx, p->name);
}

static void
axa_doc(Mloc *mp, ATF *a)
{
  if (a->edoc == EDOC_COMPOSITE)
    fprintf(axa_fp, "@composite\n");
  else if (a->edoc == EDOC_SCORE)
    fprintf(axa_fp, "@score %s %s%s\n",
	    a->stype == EDOC_MATRIX ? "matrix" : "synoptic",
	    a->sparse == EDOC_PARSED ? "parsed" : "unparsed",
	    a->sword ? " word" : "");  
}

static void
axa_lang(Mloc *mp, ATF *a)
{
  if (a->altlang)
    fprintf(axa_fp, "#atf: lang %s _%s_\n", a->lang, a->altlang);
  else
    fprintf(axa_fp, "#atf: lang %s\n", a->lang);
}

static void
axa_dollar(Mloc *mp, void *p)
{
}

static void
axa_protocol(Mloc *mp, Protocol *p)
{
  switch (p->t)
    {
    case PROT_PROJECT:
      fprintf(axa_fp, "#project: %s\n", p->u.str);
      break;
    case PROT_ATF:
      fprintf(axa_fp, "#atf: %s\n", p->u.str);
      break;
    case PROT_VERSION:
      fprintf(axa_fp, "#version: %s\n", (ccp)p->u.str);
      break;
    case PROT_LZR_SPARSE:
      {
	int n;
	const char **kk = hash_keys2(p->u.sparse, &n);
	char *v = vec_to_str((char**)kk, n, " ");
	fprintf(axa_fp, "#lemmatizer: do sparse %s\n", v);
	free(v);
      }
      break;
    case PROT_LZR_STOP:
      fprintf(axa_fp, "#lemmatizer: stop %d\n", p->u.stop);
      break;
    case PROT_TOP:
      break;
    }
}

static void
axa_xlink(Mloc *mp, Xlink *lp)
{
  if (lp->t == ELINK_DEF)
    fprintf(axa_fp, "#link: def %s = %s = %s\n", lp->siglum, lp->qid, lp->name);
  else
    fprintf(axa_fp, "#link: %s %s = %s\n", lp->t==ELINK_SOURCE ? "source" : "parallel",
	    lp->qid, lp->name);    
}

static void
axa_key(Mloc *mp, Key *kp)
{
  if (kp->val && kp->url)
    fprintf(axa_fp, "#key: %s %s %s\n", kp->key, kp->val, kp->url);
  else if (kp->val)
    fprintf(axa_fp, "#key: %s %s\n", kp->key, kp->val);
  else if (kp->url)
    fprintf(axa_fp, "#key: %s %s\n", kp->key, kp->url);
  else
    fprintf(axa_fp, "#key: %s\n", kp->key);
}

static void
axa_bib(Mloc *mp, void *p)
{
}

static void
axa_note(Mloc *mp, void *p)
{
}

static void
axa_comment(Mloc *mp, void *p)
{
}

static void
axa_block(Mloc *mp, void *p)
{
}

static void
axa_mts(Mloc *mp, void *p)
{
}

static void
axa_nts(Mloc *mp, void *p)
{
}

static void
axa_lgs(Mloc *mp, void *p)
{
}

static void
axa_gus(Mloc *mp, void *p)
{
}

static void
axa_lem(Mloc *mp, void *p)
{
}

static void
axa_llink(Mloc *mp, void *p)
{
}

static void
axa_blank(Mloc *mp, void *p)
{
}

static ltfnc lt_funcs[] = {
  (ltfnc)axa_andline,
  (ltfnc)axa_doc,
  (ltfnc)axa_lang,
  (ltfnc)axa_dollar,
  (ltfnc)axa_protocol,
  (ltfnc)axa_xlink,
  (ltfnc)axa_key,
  (ltfnc)axa_bib,
  (ltfnc)axa_note,
  (ltfnc)axa_comment,
  (ltfnc)axa_block,
  (ltfnc)axa_mts,
  (ltfnc)axa_nts,
  (ltfnc)axa_lgs,
  (ltfnc)axa_gus,
  (ltfnc)axa_lem,
  (ltfnc)axa_llink,
  (ltfnc)axa_blank
};

void
ax_atf(ATF*a)
{
  Atfl *ap;
  for (ap = list_first(a->input); ap; ap = list_next(a->input))
    lt_funcs[ap->t](&ap->l, ap->p);
}

#if 0
  if (a->project)
    fprintf(axa_fp, "#project: %s\n", a->project);
  if (a->flags)
    {
      int i, j;
      for (i = 1, j = 0; i < ATFF_TOP; i<<=1, ++j)
	if (a->flags&i)
	  fprintf(axa_fp, "#atf: use %s\n", atf_flag_str[j]);
    }
  if (a->lang)
    {
    }
  if (a->nprotocols)
    {
      int i;
      for (i = 0; i < a->nprotocols; ++i)
	ax_atf_protocol(a->protocols[i]);
    }
  if (a->nlinks)
    {
      int i;
      for (i = 0; i < a->nlinks; ++i)
	ax_atf_link(a->links[i]);
    }
  if (a->nkeys)
    {
      int i;
      for (i = 0; i < a->nkeys; ++i)
	ax_atf_key(a->keys[i]);
    }
#endif
