#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"
#include "otf-defs.h"

int already_lemmed = 0;
static int lg_mode = 0;
int mylines = 0;

struct lang_context *curr_lang_ctxt;
struct lang_context *text_lang;

/* offset used in calculation of word-ids.  In main text word-ids
   add 1..n; in exemplar 1 they add 1001 .. n+1000; in exemplar 2
   they add 2001 .. n+2000, etc. */
int exemplar_offset = 0;
int bil_offset = 0;

Hash *last_tlit_h_hash = NULL;
static struct node **last_tlit_h = NULL;
static int lth_alloced = 0;
static int lth_used = 0;
static int last_tlit_h_decay = 0;

static unsigned const char *lnstr(int number,int primes);

void
line_mts(unsigned char *lp)
{
  struct node *lnode = atf_push("l");
  /*struct attr *ap,*xid;*/
  const char *xid, *label;
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
  atf_xprop(lnode, "xml:id", (xid = (ccp)pool_copy((uccp)line_id_buf, atfmp->pool)));
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
	  hash_add(last_tlit_h_hash,(const unsigned char *)xid,tmp);
	}
      lth_used = 0;
    }

#if 0
  /* probably not used now gx has native access to lib/gdl and lib/gvl */
  if (atf_cbd_err)
    {
      extern int cbd_err_line;
      cbd_err_line = atoi((char*)tok);
    }
#endif

  if (!mylines)
    {
#if 1
      atf_xprop(lnode, "o", (ccp)pool_copy((uccp)tok, atfmp->pool));
      atf_xprop(lnode, "n", (ccp)(tok = pool_copy((uccp)lnstr(lninfo.lineno,lninfo.lineprimes),
						  atfmp->pool)));
#else
      appendAttr(lnode,attr(a_o,tok));
      tok = (unsigned char*)lnstr(lninfo.lineno,lninfo.lineprimes);
      appendAttr(lnode,attr(a_n,tok));
#endif
    }
  else
#if 1
    atf_xprop(lnode, "n", (ccp)pool_copy((uccp)tok, atfmp->pool));
#else
    appendAttr(lnode,attr(a_n,tok));
#endif
  
  curr_line_label = line_label(tok,0,(unsigned const char *)xid);
  if (curr_line_label)
    {
#if 1
      atf_xprop(lnode, "label", (label = (ccp)pool_copy((uccp)curr_line_label, atfmp->pool)));
#else
      ap = attr(a_label,curr_line_label);
      appendAttr(lnode,ap);
#endif
      extern char *label2;
      if (label2)
	{
#if 1
	  atf_xprop(lnode, "label2", (ccp)pool_copy((uccp)label2, atfmp->pool));
#else
	  ap = attr(a_label2,(unsigned const char *)label2);
	  appendAttr(lnode,ap);
#endif
	}
      register_label(labtab,uc(xid),uc(label));
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
#if 1
	  atf_xpush("spanall", "1");
#else
	  appendAttr(lnode,attr(a_spanall,(unsigned char *)"1"));
#endif
	}
      
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(s,end,lnode,1, WITH_WORD_LIST,uc(line_id_buf));
	}
    }
  else
    tlit_reinit_inline(WITH_WORD_LIST);
}

void
line_bil(unsigned char *lp)
{
  struct node *lnode = atf_push("l");
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  
  already_lemmed = 0;
  note_initialize_line();
  appendAttr(lnode,attr(a_type,ucc("bil")));
  appendChild(current,lnode);
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(s,end,lnode, 
			    1 + (++exemplar_offset * 1000), 
			    WITH_WORD_LIST, uc(line_id_buf));
	}
    }
}

/* gloss under stream */
void
line_gus(unsigned char *lp)
{
  struct node *lnode = atf_push("l");
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  
  appendAttr(lnode,attr(a_type,ucc("gus")));
  appendChild(current,lnode);
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(s,end,lnode,1001, WITH_WORD_LIST,uc(line_id_buf));
	}
    }
}

/* normalized transliteration stream */
void
line_nts(unsigned char *lp)
{
  struct node *lnode = atf_push("l");
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  
  already_lemmed = 0;
  appendAttr(lnode,attr(a_type,ucc("nts")));
  appendChild(current,lnode);
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(s,end,lnode,2001, WITH_WORD_LIST,uc(line_id_buf));
	}
    }
}

void
line_lgs(unsigned char *lp)
{
  extern int suppress_lem;
  suppress_lem = 1;
  struct node *lnode = atf_push("l");
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  appendAttr(lnode,attr(a_type,ucc("lgs")));
  while (isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(s,end,lnode, 3001, NO_WORD_LIST,uc(line_id_buf));
	}
    }  
  appendChild(current,lnode);
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
	  extern Hash *label_table;
	  char *fragid = malloc(strlen(qualid)+10);
	  (void)sprintf(fragid, "%s.%d", qualid, n);
	  if (hash_find(label_table, (const unsigned char *)fragid))
	    return fragid;
	}
    }
  return NULL;
}

void
line_var(unsigned char *lp)
{
  struct node *lnode = atf_push("v");
  unsigned char *s = lp, *entry = lp;
  unsigned char *end = lp+xxstrlen(lp);
  unsigned char *n, *n_vbar;
  unsigned char lab[128];
  struct symbolattr *sa = NULL;
  char *tmp = NULL;
  struct attr *xid = NULL;

  note_initialize_line();

  /* FIXME: stabilize this so that the same siglum maps
     to the same exemplar_offset within each text and 
     output a map of the siglum::exemplar_offset pairs */
  ++exemplar_offset;
  
  /* appendAttr(lnode,attr(a_type,ucc("var"))); */

  (void)sprintf(line_id_buf+strlen(line_id_buf),(const char *)"v%d", exemplar_offset);
  set_tr_id(line_id_buf);
  xid = attr(a_xml_id,ucc(line_id_buf));
  tmp = line_id_buf + strlen(line_id_buf);
  while (tmp[-1] != 'v')
    --tmp;
  tmp[-1] = '\0';
  appendAttr(lnode,xid);

  appendChild(current,lnode);

  while (*s && !isspace(*s))
    ++s;

  n = s-1; /* now n points at final ':' */
  *n = '\0';
  n = entry;
  if ((n_vbar = (unsigned char*)strchr((char*)n,','))) /* used to be | but now , */
    *n_vbar++ = '\0';
  appendAttr(lnode,attr(a_varnum,n));

  if (doctype == e_score)
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
	      appendAttr(lnode,attr(a_hlid,(const unsigned char *)hlid));
	      if (tmp)
		appendAttr(lnode,attr(a_fragid,(const unsigned char *)tmp));
	    }
	}
      appendAttr(lnode,attr(a_n,(const unsigned char *)sa->pname));
      appendAttr(lnode,attr(a_p,(const unsigned char *)sa->qualified_id));
    }

  sprintf((char*)lab, "%s [%s]", curr_line_label, n);
  appendAttr(lnode,attr(a_label,lab));
  if (n_vbar)
    appendAttr(lnode,attr(a_ex_label,map_uscore(n_vbar)));

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
	  tlit_parse_inline(s, end, lnode, 1 + (1000 * exemplar_offset),
			    WITH_WORD_LIST, uc(line_id_buf));
	}
    }
}

/*FIXME: test the array bounds and generate strings dynamically if 
  the line numbers are out of range */
static unsigned const char *
lnstr(int number,int primes)
{
  if (primes)
    {
      if (primes > 4)
	{
	  vwarning("%d is too many primes: restructure using @fragment\n", primes);
	  return ucc("");
	}
      else if (number > 500)
	{
	  vwarning("%d is too big!",number);
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
	  return pool_copy((unsigned char*)buf);
	}
      else
	return ucc(lnstrs[number]);
    }
}
