#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"

static void atf_bld_protocols(Mloc *lp, const char *scope);
int in_preamble;

static Tree *abt;

void
atf_bld_tree(Tree*tp)
{
  abt = tp;
}

Node *
atf_add(const char *s)
{
  return tree_add(abt, NS_XTF, s, abt->root->depth+1, NULL);
}

Node *
atf_push(const char *s)
{
  tree_add(abt, NS_XTF, s, abt->curr->depth, NULL);
  return tree_push(abt);
}

void
atf_prop(Node *ynp, int ptype, int gtype)
{
  if (ynp)
    ynp->props = prop_add(ynp->tree->tm->prop_mem, ynp->props, ptype, gtype);
  else
    mesg_warning(curratffile, atflineno, "atf_prop passed NULL ynp");
}

void
atf_prop_kv(Node *ynp, int ptype, int gtype, const char *k, const char *v)
{
  if (ynp && v)
    prop_node_add(ynp, ptype, gtype, k, v);
  else if (!v)
    mesg_vwarning(curratffile, atflineno, "atf_prop passed NULL value for key %s", v);    
  else
    mesg_warning(curratffile, atflineno, "atf_prop passed NULL ynp");
}

void
atf_bld_amp(Mloc l, const char *pqx, unsigned const char *name)
{
  atfp = atfmp->atf;
  atfp->file = curratffile;
  atfp->pqx = pqx;
  atfp->name = name;
  atfp->edoc = EDOC_TRANSLITERATION;
  Blocktok *btp = blocktok("transliteration", strlen("transliteration"));
  Node *np = atf_push(btp->name);
  abt->root->mloc = np->mloc = mloc_mloc(&l);
  abt->root->user = atfmp->atf;
  np->user = btp;
  atf_xprop(np, "xml:id", atfmp->atf->pqx);
  atf_xprop(np, "n", (ccp)atfmp->atf->name);
  atf_xprop(np, "xml:lang", "sux");
  list_add(atfmp->atf->atflines, np);
  in_preamble = 1;
}

void
atf_bld_doc(Mloc l)
{
  if (atfp->edoc != EDOC_TRANSLITERATION)
    {
      abt->curr->user = curr_blocktok;
      abt->curr->name = curr_blocktok->name;
      if (atfp->stype)
	{
	  atf_xprop(abt->curr, "score-type",
		      atfp->stype==EDOC_MATRIX ? "matrix" : "synopsis");
	  atf_xprop(abt->curr, "score-mode",
		      atfp->sparse==EDOC_PARSED ? "parsed" : "unparsed");
	  if (atfp->sword)
	    atf_xprop(abt->curr, "score-word", "yes");
	}
    }
}

void
atf_bld_group(Mloc l, Tree *tp)
{
  
}

void
atf_bld_implicit_block(void)
{
  curr_block = memo_new(atfmp->mblocks);
  /* for now curr_block->src == NULL means implicit */
  curr_group = memo_new(atfmp->mgroups);
  curr_block->lines = curr_group;
}

void
abt_add_key_protocol(Mloc *lp, Key *kp)
{
  if (strcmp(abt->curr->name, "protocols"))
    atf_bld_protocols(lp, "text");
  Node *np = atf_add("protocol");
  np->user = kp;  
  if (!strcmp(kp->key, "after") || !strcmp(kp->key, "see"))
    {
      atf_xprop(np, "type", kp->key);
      atf_xprop(np, "url", kp->url);
    }
  else
    {
      atf_xprop(np, "type", "key");
      atf_xprop(np, "kkey", kp->key);
      np->text = kp->val;
    }
}

static void
abt_add_link_protocol(Mloc *lp, Xlink *p, const char *str)
{
  if (strcmp(abt->curr->name, "protocols"))
    atf_bld_protocols(lp, "text");
  Node *np = atf_add("protocol");
  atf_xprop(np, "type", "link");
  np->text = str;
  np->user = lp;  
}

void
atf_bld_link(Mloc l, Linkt lt, const unsigned char *siglum, const char *qid,
	     const unsigned char *name)
{
  Xlink *lp = memo_new(atfmp->mxlinks);
  lp->t = lt;
  lp->siglum = siglum;
  lp->qid = qid;
  lp->name = name;
  list_add(atfmp->llinks, lp);
  const char *ltstr = (lt==ELINK_DEF?"def":(lt==ELINK_SOURCE?"source":"parallel"));
  char str[strlen(ltstr)+(siglum?strlen((ccp)siglum):0)+strlen(qid)+strlen((ccp)name)+4];
  if (siglum)
    sprintf(str, "%s %s = %s = %s", ltstr, (ccp)siglum, qid, (ccp)name);
  else
    sprintf(str, "%s %s = %s", ltstr, qid, (ccp)name);
  abt_add_link_protocol(&l, lp, (ccp)pool_copy((uccp)str, atfmp->pool));
}

static void
abt_add_protocol(Mloc *lp, Protocol *p, const char *scope, const char *str)
{
  if (strcmp(abt->curr->name, "protocols"))
    atf_bld_protocols(lp, scope);
  Node *np = atf_add("protocol");
  atf_xprop(np, "type", p->type);
  np->text = str;
  np->user = p;
}

static void
atf_bld_protocols(Mloc *lp, const char *scope)
{
  Node *np = atf_push("protocols");
  atf_xprop(np, "scope", scope);
}

void
atf_bld_protocol(Mloc l, Prot pt, const char *str)
{
  Protocol *p = memo_new(atfmp->mprotocols);
  p->t = pt;
  switch (pt)
    {
    case PROT_BIB:
      p->type = "bib";
      p->u.str = (uccp)str;
      break;
    case PROT_NOTE:
      p->type = "note";
      p->u.str = (uccp)str;
      break;
    case PROT_VERSION:
      p->type = "version";
      p->u.str = (uccp)str;
      break;
    case PROT_LZR_SPARSE:
      {
	p->type = "lemmatizer";
	char *dup = strdup(str);
	char **ff = vec_from_str(dup, NULL, NULL);
	atfp->lzr_sparse = p->u.sparse = hash_create(0);
	int i;
	for (i = 0; ff[i]; ++i)
	  {
	    hash_add(p->u.sparse, pool_copy((uccp)ff[i], atfmp->pool), "");
	    free(ff[i]);
	  }
	free(ff);
	free(dup);
      }
      break;
    case PROT_LZR_STOP:
      p->type = "lemmatizer";
      p->u.stop = atoi(str);
      break;
    case PROT_PROJECT:
      p->type = "project";
      break;
    case PROT_ATF:
    case PROT_TOP:
      break;
    }
  list_add(atfmp->lprotocols, p);
  abt_add_protocol(&l, p, in_preamble ? "text" : "intra", str);
}

void
atf_bld_atf_protocol(Mloc l, int usetype, const char *str)
{
  Protocol *p = memo_new(atfmp->mprotocols);
  p->t = PROT_ATF;
  p->type = "atf";
  abt_add_protocol(&l, p, "text", str);
}

void
atf_bld_column(Mloc l, Blocktok *curr_blocktok)
{
}

void
atf_bld_division(Mloc l, Blocktok *curr_blocktok)
{
}

void
atf_bld_heading(Mloc l, Blocktok *curr_blocktok)
{
}

void
atf_bld_milestone(Mloc l, Blocktok *curr_blocktok)
{
}

void
atf_bld_object(Mloc l, Blocktok *curr_blocktok)
{
}

void
atf_bld_surface(Mloc l, Blocktok *curr_blocktok)
{
}

void
atf_bld_mts(Mloc l, int linetype, const char *linetext)
{
  /* check current tree parent and build object/surface/column as
   * implicit levels if necessary
   */
}

void
atf_bld_xxx(Mloc l, int linetype, const char *linetext)
{
  
}
