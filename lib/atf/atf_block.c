#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"
#include "atf.tab.h"
#include "blocktok.h"
#include "otf-defs.h"

static void block_div(Mloc l, Blocktok *btp, char *rest);
static void block_hdr(Mloc l, Blocktok *btp, char *rest);

int div_level;
int header_id;

unsigned char idbuf[MAX_IDBUF_LEN+1];
char line_id_buf[MAX_LINE_ID_BUF+1];

Hash *last_tlit_h_hash = NULL;
static struct node **last_tlit_h = NULL;
static int lth_alloced = 0;
static int lth_used = 0;
static int last_tlit_h_decay = 0;

#define MAX_ROMAN 40
const char * const roman[] = {
  "", 
  "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix", "x", 
  "xi", "xii", "xiii", "xiv", "xv", "xvi", "xvii", "xviii", "xix", "xx", 
  "xxi", "xxii", "xxiii", "xxiv", "xxv", "xxvi", "xxvii", "xxviii", "xxix", "xxx",
  "xxxi", "xxxii", "xxxiii", "xxxiv", "xxxv", "xxxvi", "xxxvii", "xxxviii", "xxxix", 
  "xl"
};

static void
atf_implicit(const char *n)
{
  Block *bp = memo_new(atfmp->mblocks);
  bp->implicit = 1;
  bp->bt = blocktok(n, strlen(n));
  Node *np = atf_push(n);
  np->user = bp;

  if (!strcmp(n, "object"))
    atf_xprop(np, "type", "tablet");
  else if (!strcmp(n, "surface"))
    atf_xprop(np, "type", "obverse");
  else if (!strcmp(n, "column"))
    atf_xprop(np, "n", "0");
  atf_xprop(np, "implicit", "1");
}

static void
m_types(const char *s, const char **typep, const char **subtp)
{
  char *p = (char*)pool_copy((uccp)s, atfmp->pool);
  *typep = p;
  while (*p && !isspace(*p))
    ++p;
  if (*p)
    *p++ = '\0';
  *subtp = p;
  while (*p && !isspace(*p))
    ++p;
  if (*p)
    *p = '\0';      
}

/* Find the ancestor-or-self which is a block token at the level of arg2;
 * if not found, set tree_curr to the first ancestor that wasn't a block.
 */
static Node *
ancestor_or_self_level(Node *np, Block_level b)
{
  while (1)
    {
      if (!np->user)
	break;
      if (((Block*)np->user)->bt->type == b)
	return np;
      else if (np->rent)
	{
	  np = np->rent;
	  if (!blocktok(np->name, strlen(np->name)))
	    break;
	}
      else
	break;
    }
  tree_curr(np);
  return NULL;
}

/* Ensure that abt->curr is appropriate for attaching the new block */
static void
set_block_curr(Block *bp)
{
  if (EDOC_COMPOSITE == atfp->edoc)
    tree_curr(node_ancestor_or_self(abt->curr, "composite"));
  else
    {
      switch (bp->bt->type)
	{
	case B_OBJECT:
	  tree_curr(abt->root->kids); /* add '->last' here? */
	  break;
	case B_SURFACE:
	  {
	    Node *np = ancestor_or_self_level(abt->curr, B_OBJECT);
	    if (!np)
	      atf_implicit("object");
	    else
	      tree_curr(np);
	  }
	  break;
	case B_COLUMN:
	  {
	    Node *np = ancestor_or_self_level(abt->curr, B_SURFACE);
	    if (!np)
	      {
		Node *op = ancestor_or_self_level(abt->curr, B_OBJECT);
		if (!op)
		  atf_implicit("object");
		atf_implicit("surface");
	      }
	    else
	      tree_curr(np);
	  }  
	  break;
	case B_H1:
	case B_H2:
	case B_H3:
	case B_MILESTONE:
	  {
	    Node *np = ancestor_or_self_level(abt->curr, B_COLUMN);
	    if (!np)
	      {
		Node *sp = ancestor_or_self_level(abt->curr, B_SURFACE);
		if (!sp)
		  {
		    Node *op = ancestor_or_self_level(abt->curr, B_OBJECT);
		    if (!op)
		      atf_implicit("object");
		    atf_implicit("surface");
		  }
		atf_implicit("column");
	      }
	    else
	      tree_curr(np);
	  }
	  break;
	default:
	  /* other types may not need any reset of curr */
	  break;
	}
	
    }
}

void
atf_bld_block(Mloc l, Blocktok *btp, char *rest)
{
  Block *bp = memo_new(atfmp->mblocks);
  bp->bt = btp;

  if ('=' == *rest)
    ++rest;

  if (Y_M == btp->bison)
    {
      bp->text = rest;
      m_types(rest, &bp->type, &bp->subt);
    }
  else
    {
      /* for @object tablet? flags are at end of 'rest'
       * for @tablet? flags are at end of name token
       */
      char *f = strpbrk(rest, "*!?");
      if (f)
	{
	  bp->flag = f;
	  while ('!' == *rest || '?' == *rest || '*' == *rest)
	    ++rest;
	  if (*rest)
	    {
	      if (isspace(*rest))
		{
		  *rest++ = '\0';
		  while (isspace(*rest))
		    ++rest;
		  if (*rest)
		    bp->text = rest;
		}
	      else
		{
		  int len = rest-bp->flag;
		  char *f = (char*)pool_alloc(1+len, atfmp->pool);
		  strncpy(f, bp->flag, len);
		  f[len] = '\0';
		  bp->flag = f;
		  bp->text = rest;
		}
	    }
	  *f = '\0';
	}
    }

  set_block_curr(bp);
  
  if (atfp->edoc == EDOC_TRANSLITERATION)
    bp->np = atf_push(bp->bt->name);
  else
    bp->np = atf_add(bp->bt->name);

  switch (bp->bt->type)
    {
    case B_OBJECT:
      if (strcmp(bp->bt->name, "object"))
	{
	  bp->np->name = "object";
	  atf_xprop(bp->np, "type", bp->bt->name);
	}
      else
	atf_xprop(bp->np, "type", rest);
      break;
    case B_SURFACE:
      if (strcmp(bp->bt->name, "surface"))
	{
	  bp->np->name = "surface";
	  atf_xprop(bp->np, "type", bp->bt->name);
	  atf_xprop(bp->np, "label", bp->bt->nano);
	  atf_xprop(bp->np, "xml:id", bp->bt->nano);
	}
      else
	atf_xprop(bp->np, "type", rest);
      break;
    case B_COLUMN:
      atf_xprop(bp->np, "label", roman[atoi(rest)]);
      break;
    case B_DIVISION:
      block_div(l, btp, rest);
      break;
    case B_H1:
    case B_H2:
    case B_H3:
      block_hdr(l, btp, rest);
      break;
    default:
      break;
    }
  
  bp->np->user = bp;
  if (bp->type)
    {
      atf_xprop(bp->np, "type", bp->type);
      if (bp->subt)
	atf_xprop(bp->np, "subtype", bp->subt);
    }
  atf_input(l, LT_BLOCK, bp);
}

static void
block_div(Mloc l, Blocktok *btp, char *rest)
{
  unsigned char *tok = NULL, save = '\0';
#define current abt->curr
  while (((Block*)current->user)->bt->type != B_DIVISION
	 && ((Block*)current->user)->bt->type != B_TEXT)
    current = current->rent;
  Node *np = atf_push(btp->name); /*appendChild(current,elem(tag,NULL,lnum,bp->type));*/
  while (*rest && !isspace(*rest))
    ++rest;
  if (*rest)
    ++rest;
  while (isspace(*rest))
    ++rest;
  if (*rest)
    {
      int segflag = 0;
      int tabflag = 0;
      int verflag = 0;
      int non_label_div = 0;
      unsigned char *divtok = NULL, *ntok = NULL;
      ++div_level;
      tok = (unsigned char *)rest;
      while (*rest && !isspace(*rest))
	++rest;
      save = *rest;
      *rest = '\0';
      atf_xprop(np, "type", (ccp)(divtok = pool_copy((uccp)tok, atfmp->pool))); /*appendAttr(current, attr(a_type,tok));*/
      if (!xstrcmp(tok,"segment"))
	segflag = 1;
      else if (!xstrcmp(tok,"tablet"))
	tabflag = 1;
      else if (!xstrcmp(tok,"version"))
	verflag = 1;
      else if (!xstrcmp(tok,"kirugu")
	       || !xstrcmp(tok,"trailer")
	       || !xstrcmp(tok,"jicgijal"))
	non_label_div = 1;
      else
	/*divtok = pool_copy(tok)*/; /* divtok now set in atf_xprop call above */
      *rest = save;
      if (*rest)
	{
	  while (isspace(*rest))
	    ++rest;
	  tok = (unsigned char*)rest;
	  rest = rest+xxstrlen(rest);
	  while (isspace(rest[-1]))
	    --rest;
	  if (*rest)
	    *rest = '\0';
	  atf_xprop(np, "n", (ccp)(ntok = pool_copy(tok, atfmp->pool))); /*appendAttr(current,attr(a_n,tok));*/
	}
      else
	ntok = (unsigned char *)"";
      if (verflag && xstrcmp(ntok,"0"))
	{
	  unsigned char *tokend = ntok;
	  while (*tokend && !isspace(*tokend))
	    ++tokend;
	  *tokend = '\0';
	  label_segtab("Ver.",ntok);
	}
      if (!non_label_div)
	{
	  if (segflag && xstrcmp(ntok,"0"))
	    label_segtab("Seg.",ntok);
	  else if (tabflag && xstrcmp(ntok,"0"))
	    label_segtab("Tab.",ntok);
	  else if (divtok)
	    label_segtab(cc(divtok),ntok);
	}
    }
  else if (!xstrcmp(btp->name,"variants")) 
    {
      /*AX: this is already taken care of at start of function */
      /*setName(current,e_variants);*/
      /*current = appendChild(current,elem(e_variant,NULL,lnum,bp->type));*/
    } 
  else if (!xstrcmp(btp->name,"variant"))
    {
#if 1
      /* looks like ox removed the 'variant' then made @variants the
	 parent and re-added it; with ax that probably isn't
	 necessary; just check @variant has a parent @variants */
      if (strcmp(np->rent->name, "variants"))
	warning("orphan @variant");
#else
      struct node *parent = current->parent;
      (void)removeLastChild(parent);
      current = parent;
      while (current && xstrcmp(current->names->pname,"variants"))
	current = current->parent;
      if (!current)
	{
	  warning("orphan @variant (not preceded by '@variants')");
	  return;
	}
      else
	current = appendChild(current,elem(e_variant,NULL,lnum,bp->type));
#endif
    }
  else
    warning("@div must give division type");
#undef current
}

static void
block_hdr(Mloc l, Blocktok *btp, char *rest)
{
  Node *np = atf_add("h");

  if ((lth_used+1) >= lth_alloced)
    {
      lth_alloced += 8;
      last_tlit_h = realloc(last_tlit_h, lth_alloced * sizeof(struct node *));
    }
  last_tlit_h[lth_used++] = abt->curr;
  last_tlit_h_decay = 1;

  atf_xprop(np, "level", (btp->name[1]=='1'?"1":(btp->name[1]=='2'?"2":"3")));
  sprintf((char *)idbuf,"%s.h%d",textid,header_id++);
  atf_xprop(np, "xml_id", (ccp)pool_copy(idbuf, atfmp->pool));
  while (isspace(*rest))
    ++rest;
#if 1
  np->text = rest;
#else
  htext = rest;
  /* this really needs to be a parsed para */
  (void)trans_inline(current,htext,NULL,0);
#endif
}

#if 0
static void
block(unsigned char *rest,unsigned char *eol,struct block_token *bp)
{
  enum e_type tag;
  register unsigned char *tok = NULL;
  unsigned char save, *htext = NULL;

  switch (tag)
    {
    case e_div:
    case e_enum_top:
      while (*line && !isspace(*line))
	++line;	
      if (line < eol)
	++line;
      while (isspace(*line))
	++line;
      if (*line)
	{
	  if (tok && (!xstrcmp(tok,"body")
		      || !xstrcmp(tok,"catchline")
		      || !xstrcmp(tok,"colophon")
		      || !xstrcmp(tok,"date")
		      || !xstrcmp(tok,"linecount")
		      || !xstrcmp(tok,"sealings")
		      || !xstrcmp(tok,"signature")
		      || !xstrcmp(tok,"summary")
		      || !xstrcmp(tok,"witnesses")))
	    {
	      if (!xstrcmp(tok,curr_discourse))
		appendChild(current,end_discourse(tok));
	      else
		vwarning("mismatched milestones: found `@%s' expecting `@end %s'",
			 tok,curr_discourse);
		
	    }
	  else
	    {
	      while (current->level != DIVISION && current->level != TEXT)
		current = current->parent;
	      --div_level;
	      tok = line;
	      while (*line && !isspace(*line))
		++line;
	      save = *line;
	      *line = '\0';
	      if (!xstrcmp(tok,getAttr(current,"type")))
		{
		  *line = save;
		  if (*line)
		    {
		      while (isspace(*line))
			++line;
		      if (*line)
			{
			  register unsigned char *tok2 = line;
			  line = line + xxstrlen(line);
			  while (isspace(line[-1]))
			    --line;
			  if (!xstrcmp(tok2,getAttr(current,"n")))
			    current = current->parent;
			  else
			    {
			      static char type[16],n[16];
			      xstrcpy(type,getAttr(current,"type"));
			      xstrcpy(n,getAttr(current,"n"));
			      vwarning("mismatched strict @div %s %s ... @end %s %s",
				       type,n,
				       tok, tok2);
			    }
			}
		      else
			current = current->parent;
		    }
		  else
		    current = current->parent;
		}
	      else if (current_trans && !xstrcmp(tok,"translation"))
		{
		  /* @translation ... @end translation is OK */
		  if (current_trans)
		    trans_cols_attr(current_trans);
		}
	      else
		vwarning("mismatched @div %s ... @end %s",
			 getAttr(current,"type"),tok);
	    }
	}
      else if (!xstrcmp(btp->name,"endvariants"))
	{
	  while (current && xstrcmp(current->names->pname,"variants"))
	    current = current->parent;
	  if (!current)
	    {
	      warning("orphan @endvariants (not preceded by '@variants')");
	      return;
	    }
	  else
	    current = current->parent;
	}
      else
	warning("@end must give division type");
      break;
    case e_h:
    default:
      if (doctype == e_composite)
	current = c_attach_point();
      else
	current = attach_point(bp->type - 1);
      current = appendChild(current,elem(tag,NULL,lnum,bp->type));
      if (flags != f_none && bp->type != COLUMN)
	set_flags(current,&flag_info[flags]);
      switch (bp->type)
	{
	case OBJECT:
	  setAttr(current,a_type,ucc(bp->name));
	  if (!xstrcmp(enames[tag].pname,bp->name))
	    {
	      unsigned char *l = ntoken(line,eol,1,a_n);
	      if (l && *l)
		{
		  unsigned char *new_l = l;
		  if (!xstrncmp(l,"fragment",8))
		    {
		      static unsigned char nfrg[16] = { "frg." };
		      l+=8;
		      while (isspace(*l))
			++l;
		      xstrcpy(nfrg+4,l);
		      new_l = nfrg;
		    }
		  else
		    new_l = l;
		  current->user = newlabel('n',OBJECT,new_l);
		  update_labels(current,transtype);
		}
	    }
	  else
	    {
	      current->user = newlabel('b',OBJECT,bp);
	      update_labels(current,transtype);
	    }
	  break;
	case SURFACE:
	  setAttr(current,a_type,ucc(bp->name));
#if 0
	  if (block_tok_name)
	    {
	      setAttr(current,a_n,ucc(block_tok_name));
	      block_tok_name = NULL;
	    }
#endif
	  if (!xstrcmp(enames[tag].pname,bp->name)
	      || !xstrcmp(bp->name,"face"))
	    {
	      unsigned char *l = NULL;

	      if (block_tok_save)
		{
		  l = ntoken(line,eol,1,a_n);
		  if (l && *l)
		    {
		      if (!xstrcmp(bp->name,"face") 
			  && (l[1] || !islower(l[0])))
			vwarning("%s: prism's @face must be single lowercase letter; ('@surface face %s' is legal)",l,l);
		      current->user = newlabel('n',SURFACE,l);
		      update_labels(current,transtype);
		    }
		}
	      else
		{
		  vwarning("expected surface type after @surface");
		  
		}
	    }
	  else
	    {
	      unsigned char *l = ntoken(line,eol,1,a_n);
	      if (l && *l)
		{
		  if (!strcmp(bp->n,"edge"))
		    {
		      char lbuf[128], *lp = (char*)l;
		      while (*lp)
			{
			  if (!islower(*lp))
			    break;
			  else
			    ++lp;
			}		      
		      if (*lp)
			warning("designation of @edge must be lowercase letter");
		      else
			{
			  sprintf(lbuf,"%s%s",bp->nano,l);
			  current->user = newlabel('n',SURFACE,pool_copy((unsigned char *)lbuf));
			  update_labels(current,transtype);
			}
		    }
		  else if (!strcmp(bp->n,"seal") || !strcmp(bp->n,"docket"))
		    {
		      char lbuf[128], *lp;
		      lp = (char*)l + strlen(cc(l));
		      while (isspace((unsigned char)lp[-1]))
			--lp;
		      if (*lp)
			*lp = '\0';
		      sprintf(lbuf,"%s %s",bp->n,l);
		      current->user = newlabel('n',SURFACE,pool_copy((unsigned char *)lbuf));
		      update_labels(current,transtype);
		    }
		  else
		    vwarning("%s: block token %s does not take qualifiers",
			     l, bp->n);
		}
	      else
		{
		  if (!strcmp(bp->n,"seal"))
		    vwarning("@seal must be followed by a label (say `@seal 1')");
		  current->user = newlabel('b',SURFACE,bp);
		  update_labels(current,transtype);
		}
	    }
	  break;
	case COLUMN:
	  /* set o or n to user column number */
	  (void)ntoken(line, eol, 0, mylines ? a_n : a_o);
	  if (!mylines)
	    appendAttr(current,attr(a_n,lnstr(lninfo.colno,lninfo.colprimes)));
	  /* if ('1' != *(getAttr(current,"implicit"))) */
	  update_labels(current,transtype);
	  break;
	case LINE:
	default:
	  abort();
	}
    }
}

#endif
