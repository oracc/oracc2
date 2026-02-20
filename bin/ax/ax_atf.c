#include <oraccsys.h>
#include <atf.h>
#include "ax.h"

static const char *atf_flag_str[] = { "mylines" , "alignment-groups", "math",
				      "unicode", "legacy", "lexical", NULL };
static void ax_atf_key(Key *kp);
static void ax_atf_link(Xlink *lp);
static void ax_atf_protocol(Protocol *p);

void
ax_atf(ATF*a)
{
  fprintf(stderr, "&%s = %s\n", a->pqx, a->name);
  if (a->edoc == EDOC_COMPOSITE)
    fprintf(stderr, "@composite\n");
  else if (a->edoc == EDOC_SCORE)
    fprintf(stderr, "@score %s %s%s\n",
	    a->stype == EDOC_MATRIX ? "matrix" : "synoptic",
	    a->sparse == EDOC_PARSED ? "parsed" : "unparsed",
	    a->sword ? " word" : "");
  if (a->project)
    fprintf(stderr, "#project: %s\n", a->project);
  if (a->flags)
    {
      int i, j;
      for (i = 1, j = 0; i < ATFF_TOP; i<<=1, ++j)
	if (a->flags&i)
	  fprintf(stderr, "#atf: use %s\n", atf_flag_str[j]);
    }
  if (a->lang)
    {
      if (a->altlang)
	fprintf(stderr, "#atf: lang %s _%s_\n", a->lang, a->altlang);
      else
	fprintf(stderr, "#atf: lang %s\n", a->lang);
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
}

void
ax_atf_key(Key *kp)
{
  if (kp->val && kp->url)
    fprintf(stderr, "#key: %s %s %s\n", kp->key, kp->val, kp->url);
  else if (kp->val)
    fprintf(stderr, "#key: %s %s\n", kp->key, kp->val);
  else if (kp->url)
    fprintf(stderr, "#key: %s %s\n", kp->key, kp->url);
  else
    fprintf(stderr, "#key: %s\n", kp->key);
}

void
ax_atf_link(Xlink *lp)
{
  if (lp->t == ELINK_DEF)
    fprintf(stderr, "#link: def %s = %s = %s\n", lp->siglum, lp->qid, lp->name);
  else
    fprintf(stderr, "#link: %s %s = %s\n", lp->t==ELINK_SOURCE ? "source" : "parallel",
	    lp->qid, lp->name);    
}

static void
ax_atf_pro_str(const char *name, const char *str)
{
  fprintf(stderr, "#%s: %s\n", name, str);
}

void
ax_atf_protocol(Protocol *p)
{
  switch (p->t)
    {
    case PROT_BIB:
      ax_atf_pro_str("bib", (ccp)p->u.str);
      break;
    case PROT_NOTE:
      ax_atf_pro_str("note", (ccp)p->u.str);
      break;
    case PROT_VERSION:
      ax_atf_pro_str("version", (ccp)p->u.str);
      break;
    case PROT_LZR_SPARSE:
      {
	int n;
	const char **kk = hash_keys2(p->u.sparse, &n);
	char *v = vec_to_str((char**)kk, n, " ");
	fprintf(stderr, "#lemmatizer: do sparse %s\n", v);
	free(v);
      }
      break;
    case PROT_LZR_STOP:
      fprintf(stderr, "#lemmatizer: stop %d\n", p->u.stop);
      break;
    case PROT_TOP:
      break;
    }
}
