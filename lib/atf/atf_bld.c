#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"

static void atf_bld_protocols(Mloc *lp, const char *scope);
int in_preamble;

int bld_trace = 1;

Tree *abt;

void
atf_bld_tree(Tree*tp)
{
  abt = tp;
}

Node *
atf_add(const char *s)
{
  if (bld_trace)
    fprintf(stderr, "bld: atf_add %s to parent %s\n", s, abt->curr->name);
  return tree_add(abt, NS_XTF, s, abt->root->depth+1, NULL);
}

Node *
atf_push(const char *s)
{
  if (bld_trace)
    fprintf(stderr, "bld: atf_push %s to parent %s\n", s, abt->curr->name);
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
  /*abt->root->user = atfmp->atf;*//*if we do this we need a handler for "xtf" in axjoxfnc*/
  /*np->user = btp;*/
  atf_xprop(np, "xml:id", atfmp->atf->pqx);
  atf_xprop(np, "n", (ccp)atfmp->atf->name);
  atf_xprop(np, "xml:lang", "sux");
  atf_input(l, LT_ANDLINE, atfp);
  in_preamble = 1;
  reset_labels();
  reset_lninfo();
}

void
atf_bld_bib(Mloc l, const char *ltext)
{
  Bib *b = memo_new(atfmp->mbibs);
  b->text = ltext;
  Node *np = atf_add("protocol");
  atf_xprop(np, "type", "bib");
  np->user = b;
  atf_input(l, LT_BIB, b);
}

void
atf_bld_doc(Mloc l)
{
  if (atfp->edoc != EDOC_TRANSLITERATION)
    {
      abt->curr->name = (atfp->edoc == EDOC_COMPOSITE ? "composite" : "score");
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
  atf_input(l, LT_DOC, atfp);
}

void
atf_bld_group(Mloc l, Tree *tp)
{
  
}

void
atf_bld_heading(Mloc l, int type, const char *text)
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
      if (kp->val)
	np->text = kp->val;
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
  /*np->user = lp;*/
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
  atf_input(l, LT_XLINK, lp);
}

void
atf_bld_note(Mloc l, const char *ltext)
{
  Note *n = memo_new(atfmp->mbibs);
  n->text = (uccp)ltext;
  n->xid = (ccp)pool_copy((uccp)note_create_id(), atfmp->pool);
  Node *np = atf_add("note:text");
  atf_xprop(np, "xml:id", n->xid);
  atf_xprop(np, "note:mark", "1");/*place-holder*/
  np->user = n;
  atf_input(l, LT_NOTE, n);  
}

static void
abt_add_protocol(Mloc *lp, Protocol *p, const char *scope, const char *str)
{
  if (strcmp(abt->curr->name, "protocols"))
    atf_bld_protocols(lp, scope);
  Node *np = atf_add("protocol");
  atf_xprop(np, "type", p->type);
  np->text = str;
  /*np->user = p;*/
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
      p->u.str = (uccp)str;
      break;
    case PROT_ATF:
    case PROT_TOP:
      break;
    }
  list_add(atfmp->lprotocols, p);
  abt_add_protocol(&l, p, in_preamble ? "text" : "intra", str);
  atf_input(l, LT_PROTOCOL, p);
}

void
atf_bld_atf_protocol(Mloc l, const char *str)
{
  Protocol *p = memo_new(atfmp->mprotocols);
  p->t = PROT_ATF;
  p->type = "atf";
  p->u.str = (uccp)str;
  abt_add_protocol(&l, p, "text", str);
  atf_input(l, LT_PROTOCOL, p);
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

void
atf_clear_protocols(void)
{
  if (!strcmp(abt->curr->name, "protocols"))
    (void)tree_pop(abt);
}
