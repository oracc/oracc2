#include <oraccsys.h>
#include <gdl.h>
#include <lng.h>
#include "atf.h"
#include "atf_bld.h"
#include "otf-defs.h"
#include "symbolattr.h"
#include "etcsl.h"
#include "linenums.c"

int already_lemmed = 0;
static int lg_mode = 0;
int mylines = 1;
int suppress_lem = 0;

Pool *etcsl_pool = NULL;
Hash *etcsl_labels = NULL;
const char *etcsl_lid = NULL;
int lnum_labels;

Hash *vreg;

#if 0
struct lang_context *curr_lang_ctxt;
struct lang_context *text_lang;
#endif

const char *clid;

/* offset used in calculation of word-ids.  In main text word-ids
   add 1..n; in exemplar 1 they add 1001 .. n+1000; in exemplar 2
   they add 2001 .. n+2000, etc. */
int exemplar_offset = 0;
int bil_offset = 0;

Hash *last_tlit_h_hash = NULL;
struct node **last_tlit_h = NULL;
/*static int lth_alloced = 0;*/
int lth_used = 0;
int last_tlit_h_decay = 0;

Line *curr_line, *curr_lem_host;

#if 0
static int line_lines = 0;
#endif

static unsigned const char *lnstr(Mloc *mp, int number,int primes);
static unsigned char *map_uscore(const unsigned char *vbar);

void
atf_group_wrapup(void)
{
  if (list_len(atfmp->llines))
    {
      Group *gp = memo_new(atfmp->mgroups);
      gp->utype = N_U_GROUP;
      gp->lines = (Line**)list2array_c(atfmp->llines, &gp->nlines);
      gp->parent = abt->curr->user;
#if 0
      /* this is just the lines that contribute to an <lg>, excludes $-lines */
      gp->line_lines = line_lines;
#endif
      list_free(atfmp->llines, NULL);
      atfmp->llines = NULL;
      Node *np = node_ancestor_or_self(abt->curr, "lg");
      np->user = gp;
      np->utype = N_U_GROUP;
    }
}

/* Start a new line group; if we are already in a group, wrap the
 * current group up first.
 */
void
atf_line_lg(Mloc *mp)
{
  Node *np = node_ancestor_or_self(abt->curr, "lg");
  
  if (np)
    {
      atf_group_wrapup();
      tree_curr(np->rent);
    }
  else
    set_block_curr(B_LINE);

  atf_push("lg", mp);
}

static void
register_line(Mloc l, Linet lt, Node *np, unsigned char *lp)
{
#if 0
  if (lt != LT_DOLLAR)
    ++line_lines;
#endif

  atf_input(l, lt, lp);
  if (!atfmp->llines)
    {
      if (lt == LINE_MTS)
	atfmp->llines = list_create(LIST_SINGLE);
      else
	{
	  warning("out of place line\n");
	  return;
	}
    }

  curr_line = memo_new(atfmp->mlines);
  curr_line->t = lt;
  curr_line->np = np;
  curr_line->ap = list_last(atfp->input);

  list_add(atfmp->llines, curr_line);
}

void
line_mts(Mloc l, unsigned char *lp)
{
  /* Set the context to a lg, wrapping up the current lg if
     necessary */
  atf_line_lg(&l);

  struct node *lnode = atf_node("l", &l);
  lnode->mloc = mloc_mloc(&l);
  
  register_line(l, LINE_MTS, lnode, lp);
  curr_lem_host = curr_line;

  const char *label;
  unsigned char *tok = lp;
  unsigned char *end = lp+xxstrlen(lp);
  unsigned char *s = lp;

  ++lninfo.lineno;

  already_lemmed = 0;
  bil_offset = exemplar_offset = 0;
  curr_lang_ctxt = text_lang;
  /*protocol_state = s_intra;*/

  while (*s && !isspace(*s))
    ++s;
  if (s[-1] != '.')
    {
      warning("malformed line number");
      warning("(line numbers must be <non-space><period><space>, e.g., `1. ')");
      return;
    }
  else if (s == lp+1)
    {
      warning("empty line number");
    }
  s[-1] = '\0';

  sprintf(line_id_insertp,"%d", ++line_id);

  /* In lg_mode the line group carries the ID that is used for word ID bases--the code here
     adds an 'l' suffix to the <l> ID then removes it so the right ID base is used for words
  */
  if (lg_mode)
    {
      (void)strcat(line_id_buf,(const char *)"l");
      /*set_tr_id(line_id_buf);*//*re-expose when translations are reimplemented*/
    }
  atf_xprop(lnode, "xml:id", (clid = (ccp)pool_copy((uccp)line_id_buf, atfmp->pool)));

  if (etcsl_labels)
    etcsl_lid = (ccp)pool_copy((uccp)line_id_buf, atfmp->pool);

  if (lg_mode)
    line_id_buf[strlen(line_id_buf)-1] = '\0';

  if (last_tlit_h_decay)
    {
      last_tlit_h_decay = 0;
      if (!last_tlit_h_decay && lth_used)
	{
	  struct node **tmp = calloc(lth_used+1,sizeof(struct node*));
	  memcpy(tmp,last_tlit_h,lth_used * sizeof(struct node *));
	  if (!last_tlit_h_hash)
	    last_tlit_h_hash = hash_create(3);
	  hash_add(last_tlit_h_hash,(const unsigned char *)clid,tmp);
	}
      lth_used = 0;
    }

  if (!mylines)
    {
      atf_xprop(lnode, "o", (ccp)pool_copy((uccp)tok, atfmp->pool));
      atf_xprop(lnode, "n", (ccp)(tok = pool_copy((uccp)lnstr(NULL, lninfo.lineno,lninfo.lineprimes),
						  atfmp->pool)));
    }
  else
    atf_xprop(lnode, "n", (ccp)pool_copy((uccp)tok, atfmp->pool));
  
  curr_line_label = line_label(tok,0,(unsigned const char *)clid);
  if (curr_line_label)
    {
      atf_xprop(lnode, "label", (label = (ccp)pool_copy((uccp)curr_line_label, atfmp->pool)));
      extern char *label2;
      if (label2)
	atf_xprop(lnode, "label2", (ccp)pool_copy((uccp)label2, atfmp->pool));

      register_label(clid,label);
      /*free((char*)curr_line_label);*//* lib/atf uses pool not strdup */
      curr_line_label = ucc(label);
    }
  /*appendChild(current,lnode);*/
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      /* a leading |<SPACE> means span all cells */
      if (*s == '|' && isspace(s[1]))
	{
	  s += 2;
	  atf_xprop(lnode, "spanall", "1");
	}
      
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(lnode,(ccp)s,1,uc(line_id_buf));
	  curr_words = curr_line->wl = gdl_get_word_list();
	}
    }
  else
    tlit_reinit_inline();
}

void
line_bil(Mloc l, unsigned char *lp)
{
  struct node *lnode = atf_node("l", &l);
  lnode->mloc = mloc_mloc(&l);

  register_line(l, LINE_BIL, lnode, lp);
  curr_lem_host = curr_line;

  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);

  already_lemmed = 0;
  note_initialize_line();
  atf_xprop(lnode,"type","bil");
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(lnode, (ccp)s,
			    1 + (++exemplar_offset * 1000),
			    uc(line_id_buf));
	  curr_words = curr_line->wl = gdl_get_word_list();
	}
    }
}

/* gloss under stream */
void
line_gus(Mloc l, unsigned char *lp)
{
  struct node *lnode = atf_node("l", &l);

  register_line(l, LINE_GUS, lnode, lp);
  
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  
  atf_xprop(lnode,"type","gus");
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(lnode, (ccp)s, 1001, uc(line_id_buf));
	}
    }
}

/* normalized transliteration stream */
void
line_nts(Mloc l, unsigned char *lp)
{
  struct node *lnode = atf_node("l", &l);
  lnode->mloc = mloc_mloc(&l);

  register_line(l, LINE_NTS, lnode, lp);
  curr_lem_host = curr_line;
  
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  
  already_lemmed = 0;
  atf_xprop(lnode,"type","nts");
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(lnode, (ccp)s, 2001, uc(line_id_buf));
	  curr_words = curr_line->wl = gdl_get_word_list();
	}
    }
}

void
line_lgs(Mloc l, unsigned char *lp)
{
  extern int suppress_lem;
  suppress_lem = 1;

  struct node *lnode = atf_node("l", &l);
  lnode->mloc = mloc_mloc(&l);

  register_line(l, LINE_LGS, lnode, lp);
  
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  atf_xprop(lnode,"type","lgs");
  while (isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(lnode, (ccp)s, 3001, uc(line_id_buf));
	}
    }  
  suppress_lem = 0;
}

char *
compute_fragid(const char *qualid, const char *hlid)
{
  int n = atoi(strrchr(hlid,'.')+1);
  if (n > 5)
    {
      for (n -= 5; n > 0; --n)
	{
	  char *fragid = malloc(strlen(qualid)+10);
	  (void)sprintf(fragid, "%s.%d", qualid, n);
	  if (hash_find(atfp->hlabmap, (const unsigned char *)fragid))
	    return fragid;
	}
    }
  return NULL;
}

static void
v_register(const char *id)
{
  if (!id)
    {
      hash_free(vreg, NULL);
      vreg = NULL;
      return;
    }

  if (!vreg)
    vreg = hash_create(128);
  char *v = hash_find(vreg, (uccp)id);
  if (!v)
    hash_add(vreg, pool_copy((uccp)id, atfmp->pool), (void*)(uintptr_t)1);
  else
    hash_add(vreg, (uccp)id, ++v);
}

/* This is called for both exemplar (^A: ) and variant (^:31v1.1: ) */
void
line_var(Mloc l, unsigned char *lp)
{
  struct node *lnode = atf_node("v", &l);
  lnode->mloc = mloc_mloc(&l);

  register_line(l, LINE_EXX, lnode, lp);
  
  unsigned char *s = lp, *entry = lp;
  unsigned char *end = lp+xxstrlen(lp);
  unsigned char *n, *n_vbar;
  unsigned char lab[128];
  struct symbolattr *sa = NULL;
  char *tmp = NULL;

  note_initialize_line();

  /* FIXME: stabilize this so that the same siglum maps
     to the same exemplar_offset within each text and 
     output a map of the siglum::exemplar_offset pairs */
  ++exemplar_offset;
  
  /* appendAttr(lnode,attr(a_type,ucc("var"))); */

  v_register(line_id_buf);
  
  (void)sprintf(line_id_buf+strlen(line_id_buf),(const char *)"v%d", exemplar_offset);
  /*set_tr_id(line_id_buf);*/
  atf_xprop(lnode, "xml:id", (clid = (ccp)pool_copy((uccp)line_id_buf, atfmp->pool)));
  tmp = line_id_buf + strlen(line_id_buf);
  while (tmp[-1] != 'v')
    --tmp;
  tmp[-1] = '\0';

  while (*s && !isspace(*s))
    ++s;

  n = s-1; /* now n points at final ':' */
  *n = '\0';
  n = entry;
  if ((n_vbar = (unsigned char*)strchr((char*)n,','))) /* used to be | but now , */
    *n_vbar++ = '\0';
  atf_xprop(lnode,"varnum",(ccp)pool_copy((uccp)entry, atfmp->pool));

  if (atfp->edoc == EDOC_SCORE)
    sa = symbolattr_get(textid, (const char*)n);

  if (sa)
    {
      const char *hlid = NULL;
      if (n_vbar)
	{
	  hlid = label_to_id(sa->qualified_id, (const char *)n_vbar);
	  if (hlid)
	    {
	      char *tmp = compute_fragid(sa->qualified_id, hlid);
	      atf_xprop(lnode,"hlid",(ccp)pool_copy((uccp)hlid, atfmp->pool));
	      if (tmp)
		atf_xprop(lnode,"fragid",(ccp)pool_copy((uccp)tmp, atfmp->pool));
	    }
	}
      atf_xprop(lnode,"n",sa->pname);
      atf_xprop(lnode,"p",sa->qualified_id);
    }

  sprintf((char*)lab, "%s [%s]", curr_line_label, n);
  atf_xprop(lnode,"label",(ccp)pool_copy((uccp)lab, atfmp->pool));
  if (n_vbar)
    atf_xprop(lnode,"ex_label",(ccp)map_uscore(n_vbar));

  s[-1] = ':';

  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(lnode, (ccp)s, 1 + (1000 * exemplar_offset), uc(line_id_buf));
	}
    }
}

void
line_lem(Mloc ml, unsigned char *l)
{
  List *llem = list_create(LIST_SINGLE);
  while (*l)
    {
      list_add(llem, l);
      while (*l && (*l != ';' || ('+' == l[-1] || '-' == l[-1] 
                                  || (l[1] && !isspace(l[1])))))
        ++l;
      if (';' == *l)
        {
          *l++ = '\0';
          while (isspace(*l))
            ++l;
          if ('\0' == *l)
            warning("spurious ';' at end of #lem: line");
          else if (';' == *l)
            warning("empty lemmatization in #lem: line");
        }
    }

  if (list_len(llem) != list_len(curr_lem_host->wl))
    {
      if (list_len(llem) > list_len(curr_lem_host->wl))
        warning("too many lemmata");
      else
        warning("too few lemmata");      
    }
  else
    {
      Node *w;
      uccp l;
      for (w = list_first(curr_lem_host->wl), l = list_first(llem);
	   w && l;
	   w = list_next(curr_lem_host->wl), l = list_first(llem))
	{
	  gdl_prop_kv(w, GP_ATTRIBUTE, PG_GDL_INFO, "lem",
		      (ccp)pool_copy((uccp)l, gdlpool));
	  w->user = atf_save_lem(w,(ccp)l); /* should be safe because GDL doesn't put data in g:w->user */
	}
    }
  list_free(curr_lem_host->wl, NULL);
  curr_lem_host->wl = NULL; /* this is OK as long as we don't overload use of line->wl */
}

/*FIXME: test the array bounds and generate strings dynamically if 
  the line numbers are out of range */
static unsigned const char *
lnstr(Mloc *mp, int number,int primes)
{
  if (primes)
    {
      if (primes > 4)
	{
	  mesg_verr(mp, "%d is too many primes: restructure using @fragment\n", primes);
	  return ucc("");
	}
      else if (number > 500)
	{
	  mesg_verr(mp, "%d is too big!",number);
	  return ucc("");
	}
      else
	return ucc(lnstrsp[primes-1][number]);
    }
  else
    {
      if (number > 4999)
	{
	  /* vwarning("%d is too big!",number); */
	  char buf[10];
	  sprintf(buf,"%d",number);
	  return pool_copy((unsigned char*)buf, atfmp->pool);
	}
      else
	return ucc(lnstrs[number]);
    }
}

static unsigned char *
map_uscore(const unsigned char *vbar)
{
  unsigned char *tmp = pool_copy(vbar, atfmp->pool), *s;
  for (s = tmp; *s; ++s)
    if (*s == '_')
      *s = ' ';
  return tmp;
}
