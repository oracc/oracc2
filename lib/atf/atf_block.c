#include <oraccsys.h>
#include <mesg.h>
#include <scan.h>
#include "atf.h"
#include "atf_bld.h"
#include "atf.tab.h"
#include "blocktok.h"
#include "etcsl.h"
#include "otf-defs.h"

static void block_div(Mloc l, Block *bp, char *rest);
static void block_hdr(Mloc l, Block *bp, char *rest);
static void block_lev(Mloc l, Block *bp, char *rest);
static char *scan_flags(Mloc l, const char *s, char *f);
static int at_prime(unsigned char *s, unsigned char **p);
static void atf_implicit(const char *n);
static void m_types(const char *s, const char **typep, const char **subtp);
static char *obj_args(Mloc l, Block *bp, char *s, char flags[]);
static char *srf_args(Mloc l, Block *bp, char *s, char flags[]);
static char *frg_args(Mloc l, Block *bp, char *s, char flags[]);
static char *col_args(Mloc l, Block *bp, char *s, char flags[]);
static const char *xid_block(void);

static Node *ancestor_or_self_level_as(Node *np, Block_level b, int auto_set);

static int xid = 0;

static char block_flags[256] = { ['?'] = 1, ['!'] = 1 , ['*'] = 1 };
const char *primes[] = {"′","″","‴","⁗","⁗′","⁗″","⁗‴","⁗⁗"};
const char *Primes[] = {"p", "P", "Pp", "PP", "PPp", "PPP", "PPPp", "PPPP" };

#define MAX_FLAGS 5
#define MAX_PRIMES 9 /* character count for normalized ' chars */

int div_level;
int header_id;

Lninfo lninfo;
static int m_object_index = 0;

unsigned char idbuf[MAX_IDBUF_LEN+1];

extern Hash *last_tlit_h_hash;
extern struct node **last_tlit_h;
static int lth_alloced = 0;
extern int line_trace;
extern int lth_used;
extern int last_tlit_h_decay;

#define MAX_ROMAN 40
const char * const roman[] = {
  "", 
  "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix", "x", 
  "xi", "xii", "xiii", "xiv", "xv", "xvi", "xvii", "xviii", "xix", "xx", 
  "xxi", "xxii", "xxiii", "xxiv", "xxv", "xxvi", "xxvii", "xxviii", "xxix", "xxx",
  "xxxi", "xxxii", "xxxiii", "xxxiv", "xxxv", "xxxvi", "xxxvii", "xxxviii", "xxxix", 
  "xl"
};

void
atf_block_xid(void)
{
  xid = 0;
}

/* Do generic setup for @-block token and then pass control to
   block-level/type specific handlers */
void
atf_bld_block(Mloc l, Blocktok *btp, char *rest)
{
  atf_group_wrapup();

  if (line_trace)
    atf_lex_line_trace(&l);
  
  Block *bp = memo_new(atfmp->mblocks);
  bp->utype = N_U_BLOCK;
  bp->bt = btp;
  bp->literal = (ccp)pool_copy((uccp)rest, atfmp->pool);

  if ('=' == *rest)
    ++rest;

  if (bp->bt->type == B_FRAGMENT)
    {
      Node *np = ancestor_or_self_level_as(abt->curr, B_COLUMN, 0);
      if (!np)
	atfp->frag_type = F_SURFACE;
      else
	atfp->frag_type = F_SUBSURF;
    }
  
  /* Take care not to create a div np in this routine because
     block_div handles that and does div/end div checking */
  if (bp->bt->type != B_DIVISION)
    {
      set_block_curr(bp->bt->type);

      if (bp->bt->type == B_MILESTONE
	  || bp->bt->bison == Y_FRAGMENT)
	bp->np = atf_add("m", &l);
      else if (atfp->edoc == EDOC_TRANSLITERATION)
	bp->np = atf_push(bp->bt->name, &l);
      else
	bp->np = atf_add(bp->bt->name, &l);
      bp->np->user = bp;
    }

  atf_input(l, LT_BLOCK, bp);

  switch (bp->bt->type)
    {
    case B_OBJECT:
      xid_block();
      reset_lninfo();
      block_lev(l, bp, rest);
      break;
    case B_SURFACE:
      reset_lninfo();
      block_lev(l, bp, rest);
      break;
    case B_FRAGMENT:
      reset_lninfo();
      block_lev(l, bp, rest);
      break;
    case B_COLUMN:
      ++lninfo.colno;
      lninfo.lineno = lninfo.lineprimes = 0;
      block_lev(l, bp, rest);
      break;
    case B_DIVISION:
      reset_lninfo();
      block_div(l, bp, rest);
      break;
    case B_H1:
    case B_H2:
    case B_H3:
      block_hdr(l, bp, rest);
      break;
    default:
      break;
    }
}

/* bp is Block and rest is the remainder of the @-line.

   (replacement for otf block.c:ntoken())
 */
static void
block_lev(Mloc l, Block *bp, char *rest)
{
  char *s = rest;
  char flags[MAX_FLAGS+1] = { '\0' };

  while (isspace(*s))
    ++s;

  /* handle @obverse? etc */
  if (block_flags[(unsigned char)*s])
    s = scan_flags(l, s, flags);

  switch (bp->bt->type)
    {
    case B_OBJECT:
      if (*flags && !strcmp(bp->bt->name, "object"))
	mesg_verr(&l, "flags not allowed after @object; should follow object type arg");
      s = obj_args(l, bp, s, flags);
      break;
    case B_SURFACE:
      if (*flags && !strcmp(bp->bt->name, "surface"))
	mesg_verr(&l, "flags not allowed after @surface; should follow surface type arg");
      s = srf_args(l, bp, s, flags);
      break;
    case B_FRAGMENT:
      atf_xprop(bp->np, "type", "locator");
      atf_xprop(bp->np, "subtype", "fragment");
      s = frg_args(l, bp, s, flags);
      if (bp->subt)
	{
	  bp->np->text = (ccp)pool_copy((uccp)bp->subt, atfmp->pool);
	  label_frag(bp->np, (uccp)bp->np->text);
	}
      break;
    case B_COLUMN:
      if (*flags)
	mesg_verr(&l, "flags not allowed after @column; should follow column number");
      if (*s)
	s = col_args(l, bp, s, flags);
      else
	mesg_verr(&l, "@column requires a column number");
      break;
    default:
      fprintf(stderr, "block_lev: internal error: unhandled block type\n");
      break;
    }
  
  /* Each case above needs to move 's' beyond the last permissible token for its type */
  while (isspace(*s))
    ++s;
  if (*s)
    mesg_vwarning(l.file, l.line, "bad character in block line at: %s",s);
}

static void
new_variant(Mloc *mp)
{
  /* always initiate a new @variant */
  Block *vp = memo_new(atfmp->mblocks);
  vp->utype = N_U_BLOCK;
  vp->bt = blocktok("variant", strlen("variant"));
  vp->np = atf_push("variant", mp);
  vp->np->user = vp;
}

static void
block_div(Mloc l, Block *bp, char *rest)
{
#define current abt->curr

  char **toks = NULL;
  char *tokx[] = { NULL , NULL , NULL };
 if (rest)
    toks = vec_from_str(rest, NULL, NULL);
  else
    toks = tokx;

  const char *atname = bp->bt->name;
  unsigned const char *divtok = (toks[0] ? pool_copy((uccp)toks[0], atfmp->pool) : NULL);
  unsigned const char *ntok = (toks[1] ? pool_copy((uccp)toks[1], atfmp->pool) : NULL);

  int i;
  for (i = 0; toks[i]; ++i)
    free(toks[i]);

  if (!strcmp(abt->curr->name, "lg"))
    {
      atf_tlit_wrapup();
      tree_pop(abt);
    }

  if (!strcmp(bp->bt->name, "variant") && !strcmp(current->name, "variant"))
    {
      if (current->kids)
	tree_pop(abt);
    }
  
  while (((Block*)current->user)->bt->type != B_DIVISION
	 && ((Block*)current->user)->bt->type != B_TEXT)
    current = current->rent;
  
  if (!strcmp(atname, "end"))
    {
      if (((Block*)current->user)->bt->type == B_TEXT)
	mesg_verr(&l, "block_div: @end before @div\n");
      else if (divtok)
	{
	  const char *type = NULL;
	  if (abt->curr->props)
	    type = prop_find_kv(abt->curr->props, "type", NULL)->u.k->v;
	  if (!type && strcmp((ccp)divtok, "variants"))
	    mesg_verr(&l, "mismatched @end %s versus untyped @div\n", divtok);
	  else if (type && strcmp(type, (ccp)divtok))
	    mesg_verr(&l, "mismatched @end %s versus @div %s\n", divtok, type);
	  else
	    (void)tree_pop(abt);
	}
    }
  else
    {
      Node *np = atf_push(bp->bt->name, &l);
      np->user = bp;
      if (divtok)
	atf_xprop(np, "type", (ccp)divtok);
      if (ntok)
	atf_xprop(np, "n", (ccp)ntok);

      if (divtok)
	{
	  int segflag = 0;
	  int tabflag = 0;
	  int verflag = 0;
	  int non_label_div = 0;
	  ++div_level;

	  if (!xstrcmp(divtok,"segment"))
	    segflag = 1;
	  else if (!xstrcmp(divtok,"tablet"))
	    tabflag = 1;
	  else if (!xstrcmp(divtok,"version"))
	    verflag = 1;
	  else if (!xstrcmp(divtok,"kirugu")
		   || !xstrcmp(divtok,"trailer")
		   || !xstrcmp(divtok,"jicgijal"))
	    non_label_div = 1;
      
	  if (verflag && xstrcmp(ntok,"0"))
	    label_segtab("Ver.",ntok);
      
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
      else if (!xstrcmp(bp->bt->name,"variants")) 
	{
	  if (!strcmp(((Block*)current->rent->user)->bt->name, "variants"))
	    mesg_verr(&l, "nested @variants not allowed");
	  else
	    new_variant(&l);
	} 
      else if (!xstrcmp(bp->bt->name,"variant"))
	{
#if 0
	  /* looks like ox removed the 'variant' then made @variants
	     the parent and re-added it; for ax we require @variants
	     ... @variant ... @variants */
	  if (strcmp(current->rent->name, "variants"))
	    mesg_verr(&l, "orphan @variant");
	  else
	    {
	      /* Not sure if this happens/should be allowed */
	      mesg_verr(&l, "div within @variant\n");
	    }
#endif
	}
      else
	mesg_verr(&l, "@div must give division type");
    }
#undef current
}

static void
block_hdr(Mloc l, Block *bp, char *rest)
{
  Node *np = atf_add("h", &l);

  if ((lth_used+1) >= lth_alloced)
    {
      lth_alloced += 8;
      last_tlit_h = realloc(last_tlit_h, lth_alloced * sizeof(struct node *));
    }
  last_tlit_h[lth_used++] = abt->curr;
  last_tlit_h_decay = 1;

  atf_xprop(np, "level", (bp->bt->name[1]=='1'?"1":(bp->bt->name[1]=='2'?"2":"3")));
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

void
block_mls(Mloc l, Block *bp, char *rest)
{
  bp->literal = (ccp)pool_copy((uccp)rest, atfmp->pool);
  m_types(rest, &bp->type, &bp->subt);
  if (bp->type)
    {
      atf_xprop(bp->np, "type", bp->type);
      if (bp->subt)
	atf_xprop(bp->np, "subtype", bp->subt);
    }
}

void
reset_lninfo(void)
{
  memset(&lninfo,'\0', sizeof(Lninfo));
  m_object_index = 0;
}

static char *
scan_flags(Mloc l, const char *s, char *f)
{
  int len = 0;
  while (len < MAX_FLAGS && block_flags[(unsigned)*s])
    f[len++] = *s++;
  if (block_flags[(unsigned)*s])
    mesg_verr(&l, "ignoring excess block flags");
  f[len] = '\0';
  return (char*)s;
}

static int
at_prime(unsigned char *s,  unsigned char **p)
{
  switch (*s)
    {
    case '\'':
      *p = ++s;
      return 1;
    case '"':
      *p = ++s;
      return 2;
    case 0xe2:
      switch (s[1])
	{
	case 0x80:
	  switch (s[2])
	    {
	    case 0xb2:
	      *p = s += 3;
	      return 1;
	    case 0xb3:
	      *p = s += 3;
	      return 2;
	    case 0xb4:
	      *p = s += 3;
	      return 3;
	    }
	case 0x81:
	  if (0x97 == s[3])
	    {
	      *p = s += 3;
	      return 4;
	    }
	}
    }
  return 0;
}

/* lib/atf initially supports up to 2 quad-primes--hard to believe
   anyone could ever need more; primes are normalized to Unicode
   characters here */
int
scan_primes(const char *s, const char **endp)
{
  int len = 0;
  while (1)
    {
      int nprimes = at_prime((ucp)s, (unsigned char **)&s);
      if (nprimes)
	{
	  len += nprimes;
	  if (len >= MAX_PRIMES)
	    {
	      warning("ignoring excess primes");
	      len = 8;
	      break;
	    }
	}
      else
	break;
    }
  *endp = s;
  return len;
}

static void
atf_implicit(const char *n)
{
  Block *bp = memo_new(atfmp->mblocks);
  bp->utype = N_U_BLOCK;
  bp->implicit = 1;
  bp->bt = blocktok(n, strlen(n));
  Node *np = atf_push(n, abt->curr->mloc);
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
 * if not found, set tree_curr to the first ancestor-or-self that wasn't a block.
 */
static Node *
ancestor_or_self_level_as(Node *np, Block_level b, int auto_set)
{
  Node *non_block = NULL;
  while (1)
    {
      if (!np->user || N_U_BLOCK != ((Node*)(np->user))->utype)
	{
	  if (!non_block)
	    non_block = np;
	  if (np->rent)
	    np = np->rent;
	  else
	    break;
	}
      else if (((Block*)np->user)->bt->type == b)
	return np;
      else if (np->rent)
	np = np->rent;
      else
	break;
    }
  if (auto_set)
    {
      if (non_block)
	tree_curr(non_block);
      else
	tree_curr(np);
    }
  return NULL;
}

Node *
ancestor_or_self_level(Node *np, Block_level b)
{
  return ancestor_or_self_level_as(np, b, 1);
}

/* Ensure that abt->curr is appropriate for attaching the new block */
void
set_block_curr(Block_level b)
{

  if (EDOC_COMPOSITE == atfp->edoc || atfp->edoc == EDOC_SCORE)
    {
      Node *np = ancestor_or_self_level_as(abt->curr, B_DIVISION, 0);
      if (!np)
	np = ancestor_or_self_level(abt->curr, B_TEXT);
      if (np)
	tree_curr(np);
    }
  else
    {
      switch (b)
	{
	case B_OBJECT:
	  tree_curr(abt->root->kids); /* add '->last' here? */
	  break;
	case B_SURFACE:
	  {
	    Node *np = ancestor_or_self_level_as(abt->curr, B_OBJECT, 0);
	    if (!np)
	      atf_implicit("object");
	    else
	      tree_curr(np);
	  }
	  break;
	case B_COLUMN:
	  {
	    Node *np = ancestor_or_self_level_as(abt->curr, B_SURFACE, 0);
	    if (!np)
	      {
		Node *op = ancestor_or_self_level_as(abt->curr, B_OBJECT, 0);
		if (!op)
		  {
		    (void)ancestor_or_self_level_as(abt->curr, B_TEXT, 1);
		    if (!op)
		      atf_implicit("object");
		  }
		atf_implicit("surface");
	      }
	    else
	      tree_curr(np);
	  }  
	  break;
	case B_FRAGMENT:
	  /**@fragment is either a surface-level tag or a sub-column
	   * level one so fragments can be edited with all columns
	   * grouped together or with their text content split
	   * discontinuously over several columns. In other words, a
	   * text can have:
	  */
	  /**@tablet
	   * @fragment a
	   * @column 1
	   * @column 2
	   * @fragment b
	   * @column 1
	   * @column 2
	   */
	  /* or */
	  /**@column 1
	   * @fragment a
	   * @fragment b
	   * @column 2
	   * @fragment a
	   * @fragment b
	   */

	  if (atfp->frag_type == F_SURFACE)
	    {
	      Node *np = ancestor_or_self_level_as(abt->curr, B_OBJECT, 0);
	      if (!np)
		atf_implicit("object");
	      else
		tree_curr(np);
	      return; /* PREVENT FALL THROUGH */
	    }
	  /* FALLS THROUGH */
	case B_H1:
	case B_H2:
	case B_H3:
	case B_MILESTONE:
	case B_LINE:
	  {
	    Node *np = ancestor_or_self_level_as(abt->curr, B_COLUMN, 0);
	    if (!np)
	      {
		Node *sp = ancestor_or_self_level_as(abt->curr, B_SURFACE, 0);
		if (!sp)
		  {
		    Node *op = ancestor_or_self_level_as(abt->curr, B_OBJECT, 0);
		    if (!op)
		      {
			tree_curr(abt->root->kids);
			atf_implicit("object");
		      }
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

/* As reimplemented primes are not supported on object or surface
 * tags; are they needed for @face a′ ?
 *
 * Each of the xxx_args routines must set bp->label to the independent
 * label segment for the block; update_label computes the combined
 * label for the block in context and sets it as a property on bp->np.
 */

static char *
obj_args(Mloc l, Block *bp, char *s, char flags[])
{
  const char *stype = NULL;
  if (strcmp(bp->bt->name, "object"))
    {
      bp->np->name = "object";
      atf_xprop(bp->np, "type", stype = bp->bt->name);
    }
  else
    {
      atf_xprop(bp->np, "type", stype = scan_name(NULL, s, &s));
      s = scan_flags(l, s, flags);
    }
  if (*flags)
    bp->flag = (ccp)pool_copy((uccp)flags, atfmp->pool);

  if (bp->bt->nano)
    bp->label = (ccp)bp->bt->nano;
  else if (bp->bt->abbr)
    bp->label = (ccp)bp->bt->abbr;
  else
    bp->label = xid_block();
  
  update_label(bp->np, etu_none);

  return s;
}

static char *
srf_args(Mloc l, Block *bp, char *s, char flags[])
{
  const char *stype = NULL;
  if (strcmp(bp->bt->name, "surface"))
    {
      bp->np->name = "surface";
      atf_xprop(bp->np, "type", stype = bp->bt->name);
    }
  else
    {
      atf_xprop(bp->np, "type", stype = scan_name(NULL, s, &s));
      s = scan_flags(l, s, flags);
    }
  if (stype && !strcmp(stype, "face"))
    {
      const char *face = scan_name(NULL, s, &s);
      if (strlen(face) != 1 || !islower(*face))
	{
	  mesg_verr(&l, "face must be a single lower case letter\n");
	  face = NULL;
	}
      if (face)
	stype = bp->subt = face;
    }
  else
    {
      char *n = scan_name(NULL, s, &s);
      if (n && *n)
	{
	  atf_xprop(bp->np, "n", n);
	  int len = strlen(bp->bt->name)+strlen(n)+2;
	  bp->label = (ccp)pool_alloc(len, atfmp->pool);
	  sprintf((char*)bp->label, "%s %s", bp->bt->name, n);
	}
    }

  if (*flags)
    bp->flag = (ccp)pool_copy((uccp)flags, atfmp->pool);

  if (!bp->label)
    {
      if (bp->bt->nano)
	bp->label = (ccp)bp->bt->nano;
      else if (bp->bt->abbr)
	bp->label = (ccp)bp->bt->abbr;
      else
	bp->label = xid_block();
    }

  update_label(bp->np, etu_none);
  return s;  
}

static char *
frg_args(Mloc l, Block *bp, char *s, char flags[])
{
  const char *face = s/*scan_name(NULL, s, &s)*/;
  if (face)
    bp->subt = face;

#if 0
  if (bp->bt->nano)
    bp->label = (ccp)bp->bt->nano;
  else
    bp->label = xid_block();

  update_label(bp->np, etu_none);
#endif
  
  return s+strlen(s);  
}

static char *
col_args(Mloc l, Block *bp, char *s, char flags[])
{
  char colnum[5];
  int len = 0;
  *colnum = '\0';
  while (len < 5 && isdigit(*s))
    colnum[len++] = *s++;
  if (len == 5)
    {
      mesg_verr(&l, "column number too long (max 4 digits), ignoring excess\n");
      --len;
    }
  colnum[len] = '\0';
  if (strlen(colnum))
    {
      atf_xprop(bp->np, "n", (ccp)pool_copy((uccp)colnum, atfmp->pool));
      int rnum = atoi(colnum);
      const char *rstr = "";
      if (rnum < sizeof(roman))
	rstr = roman[rnum];
      else
	mesg_vwarning(l.file, l.line, "column number %s too big to romanize", colnum);
      const char *cprimes = "";
      int nprimes = scan_primes(s, (const char **)&s);
      if (nprimes)
	cprimes = primes[nprimes-1];
      char clabel[strlen(rstr)+strlen(cprimes)+2];
      strcpy(clabel, rstr);
      if (*cprimes)
	strcat(clabel, cprimes);
      bp->label = (ccp)pool_copy((uccp)clabel, atfmp->pool);
      update_label(bp->np, etu_none);
    }
  return s;
}

static const char *
xid_block(void)
{
  char idbuf[5];
  sprintf((char*)idbuf,"x%d",xid++);
  return (ccp)pool_copy((uccp)idbuf, atfmp->pool);
}

