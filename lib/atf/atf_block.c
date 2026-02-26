#include <oraccsys.h>
#include <mesg.h>
#include <scan.h>
#include "atf.h"
#include "atf_bld.h"
#include "atf.tab.h"
#include "blocktok.h"
#include "otf-defs.h"

static void block_div(Mloc l, Block *bp, char *rest);
static void block_hdr(Mloc l, Block *bp, char *rest);
static void block_lev(Mloc l, Block *bp, char *rest);
static char *scan_flags(const char *s, char *f);
static int at_prime(unsigned char *s, unsigned char **p);
static void atf_implicit(const char *n);
static void m_types(const char *s, const char **typep, const char **subtp);
static Node *ancestor_or_self_level(Node *np, Block_level b);
static void set_block_curr(Block *bp);
static char *obj_args(Mloc l, Block *bp, char *s, char flags[]);
static char *srf_args(Mloc l, Block *bp, char *s, char flags[]);
static char *col_args(Mloc l, Block *bp, char *s, char flags[]);

static char block_flags[128] = { ['?'] = 1, ['!'] = 1 , ['*'] = 1 };
const char *primes[] = {"′","″","‴","⁗","⁗′","⁗″","⁗‴","⁗⁗"};
const char *Primes[] = {"p", "P", "Pp", "PP", "PPp", "PPP", "PPPp", "PPPP" };

#define MAX_FLAGS 5
#define MAX_PRIMES 9 /* character count for normalized ' chars */

int div_level;
int header_id;

Lninfo lninfo;
static int m_object_index = 0;

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

/* Do generic setup for @-block token and then pass control to
   block-level/type specific handlers */
void
atf_bld_block(Mloc l, Blocktok *btp, char *rest)
{
  Block *bp = memo_new(atfmp->mblocks);
  bp->bt = btp;
  bp->literal = (ccp)pool_copy((uccp)rest, atfmp->pool);

  if ('=' == *rest)
    ++rest;

  set_block_curr(bp);

  if (atfp->edoc == EDOC_TRANSLITERATION)
    bp->np = atf_push(bp->bt->name);
  else
    bp->np = atf_add(bp->bt->name);

  bp->np->user = bp;
  atf_input(l, LT_BLOCK, bp);

  switch (bp->bt->type)
    {
    case B_OBJECT:
      reset_lninfo();
      block_lev(l, bp, rest);
      break;
    case B_SURFACE:
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
  char flags[MAX_FLAGS+1];

  while (isspace(*s))
    ++s;

  /* handle @obverse? etc */
  if (block_flags[(unsigned)*s])
    s = scan_flags(s, flags);

  switch (bp->bt->type)
    {
    case B_OBJECT:
      if (*flags && !strcmp(bp->bt->name, "object"))
	warning("flags not allowed after @object; should follow object type arg");
      s = obj_args(l, bp, s, flags);
      break;
    case B_SURFACE:
      if (*flags && !strcmp(bp->bt->name, "surface"))
	warning("flags not allowed after @surface; should follow surface type arg");
      s = srf_args(l, bp, s, flags);
      break;
    case B_COLUMN:
      if (*flags)
	warning("flags not allowed after @column; should follow column number");
      if (*s)
	s = col_args(l, bp, s, flags);
      else
	warning("@column requires a column number");
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
block_div(Mloc l, Block *bp, char *rest)
{
  unsigned char *tok = NULL, save = '\0';
#define current abt->curr
  while (((Block*)current->user)->bt->type != B_DIVISION
	 && ((Block*)current->user)->bt->type != B_TEXT)
    current = current->rent;
  Node *np = atf_push(bp->bt->name); /*appendChild(current,elem(tag,NULL,lnum,bp->type));*/
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
  else if (!xstrcmp(bp->bt->name,"variants")) 
    {
      /*AX: this is already taken care of at start of function */
      /*setName(current,e_variants);*/
      /*current = appendChild(current,elem(e_variant,NULL,lnum,bp->type));*/
    } 
  else if (!xstrcmp(bp->bt->name,"variant"))
    {
      /* looks like ox removed the 'variant' then made @variants the
	 parent and re-added it; with ax that probably isn't
	 necessary; just check @variant has a parent @variants */
      if (strcmp(np->rent->name, "variants"))
	warning("orphan @variant");
    }
  else
    warning("@div must give division type");
#undef current
}

static void
block_hdr(Mloc l, Block *bp, char *rest)
{
  Node *np = atf_add("h");

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

#if 0
/* This routine assumes flags can only occur once on a block line
   which is probably true ... */
static char *
pull_flags(char *r, const char **flagsp)
{
  char *f = strpbrk(rest, "*!?");
  if (f)
    {
      char fbuf[5], *fb = fbuf;
      *fstart = rest;
      while ('!' == *rest || '?' == *rest || '*' == *rest)
	{
	  *fb++ = *rest++;
	  if (fb - fbuf > 3)
	    {
	      fprintf(stderr, "excessive block flags ignored\n");
	      while ('!' == *rest || '?' == *rest || '*' == *rest)
		++rest;
	    }
	}
      *fb = '\0';
      if (*rest)
	{
	  while (*rest)
	    *fstart++ = *rest++;
	  *fstart = '\0';
	}
      *flagsp = (ccp)pool_copy(fbuf, atfmp->pool);
    }
  return rest;
}
#endif

void
reset_lninfo(void)
{
  memset(&lninfo,'\0', sizeof(Lninfo));
  m_object_index = 0;
}

static char *
scan_flags(const char *s, char *f)
{
  int len = 0;
  while (len < MAX_FLAGS && block_flags[(unsigned)*s])
    f[len++] = *s++;
  if (block_flags[(unsigned)*s])
    warning("ignoring excess block flags");
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
	  if (len += nprimes >= MAX_PRIMES)
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

/* As reimplemented primes are not supported on object or surface
   tags; are they needed for @face a′ ? */

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
      s = scan_flags(s, flags);
    }
  if (*flags)
    bp->flag = (ccp)pool_copy((uccp)flags, atfmp->pool);

  update_labels(bp->np, etu_top);

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
      s = scan_flags(s, flags);
    }
  if (stype && !strcmp(stype, "face"))
    {
      const char *face = scan_name(NULL, s, &s);
      if (strlen(face) != 1 || !islower(*face))
	{
	  warning("face must be a single lower case letter\n");
	  face = NULL;
	}
      if (face)
	stype = bp->subt = face;
    }

  if (*flags)
    bp->flag = (ccp)pool_copy((uccp)flags, atfmp->pool);
  
  update_labels(bp->np, etu_top);
  return s;  
}

static char *
col_args(Mloc l, Block *bp, char *s, char flags[])
{
  char colnum[5];
  int len = 0;
  *colnum = '\0';
  while (len < 5 && isdigit(*s))
    colnum[len++] = *s++;
  if (len-- == 5)
    warning("column number too long (max 4 digits), ignoring excess\n");
  colnum[len] = '\0';
  if (strlen(colnum))
    {
      int rnum = atoi(colnum);
      const char *rstr = "";
      if (rnum < sizeof(roman))
	rstr = roman[rnum];
      else
	mesg_vwarning(l.file, l.line, "column number %s too big to romanize", colnum);
      const char *cprimes = "";
      int nprimes = scan_primes(s, (const char **)&s);
      if (nprimes)
	cprimes = primes[nprimes];
      char clabel[strlen(rstr)+strlen(cprimes)+2];
      strcpy(clabel, rstr);
      if (*clabel)
	strcat(clabel, " ");
      strcat(clabel, cprimes);
      bp->label = (ccp)pool_copy((uccp)clabel, atfmp->pool);
      update_labels(bp->np, etu_top);
    }
  return s;
}

