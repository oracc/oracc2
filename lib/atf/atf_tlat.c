#include <oraccsys.h>
#include <mesg.h>
#include "atf.h"
#include "atf_bld.h"
#include "otf-defs.h"

struct translation **translations = NULL;
struct translation *current_trans;

static int multi_trans_line;
static char trans_p_idbuf[128];
static int trans_wid;
static Prop *p;
int max_trans_cols = 1;
int need_alignment = 1;
int nocellspan = 0;
int trans_parenned_labels = 1;
int trans_abbrevved_labels = 0;
static char trans_id_base[32];
static unsigned char last_xid[128];
int start_lnum = -1;
extern int next_trans_id;
static int next_trans_p_id;
extern int saa_mode;
extern unsigned const char *curr_line_label;
static unsigned char label_buf[128];
static unsigned char last_label_buf[128];

static int in_note = 0, last_label = 0;
static int need_dir_rtl = 0;

static char *tr_id_buf = NULL;

static struct node **last_trans_h = NULL, *last_p, *curr_block_np;
static int lth_alloced = 0;
static int lth_used = 0;

static void h_arefs(const unsigned char *aref);
static void se_label(struct node *np, const char *n, const char *l);

static const char *xid_prev(const char *x);
static int xid_line(const char *x);
static int xid_diff(const char *x1, const char *x2);

extern void set_tr_id(const char *id); /* can this be static? */
static void parenify(char *buf);
static void labeled_labels(struct node *p, unsigned char *lab);

static List *tral;

#if 1
#define ctr(p) atf_xprop((p),"class","tr")
#define setClass(n,c) atf_xprop((n),"class",c)
#define getAttr(n,a) ((p=prop_find_kv(n->props, a, NULL))?p->u.k->v:NULL)
#define removeAttr(n,a) prop_drop_kv(n->props, a, NULL)
#else
#define xctr(p) if (p && !*(getAttr(p,a_class))) appendAttr((p),attr(a_class,ucc("tr")))
#define yctr(p) appendAttr((p),attr(a_class,ucc("tr")))
#define ctr(p) setAttr((p),a_class,ucc("tr"))
#define setClass(n,c) appendAttr((n),attr(a_class,ucc((c))))
#define getClass(n) getAttr((n),"class")
#endif

void
atr_inter(Mloc l, unsigned char *s)
{
}

/* This is passed @(i 1) or @label i 1 */
void
atr_label(Mloc l, unsigned char *s)
{
  int innerp = 0;
  if (tral)
    atr_para(l, NULL);

  if (s[1] == '(')
    {
      /* @(i 1) */
      unsigned char *t = s+2;
      s = t;
      while (*s && ')' != *s)
	++s;
      if (*s)
	{
	  if (s - t)
	    {
	      *s = '\0';
	      xstrcpy(label_buf,normalize_ws(t));
	    }
	  else
	    innerp = 1; /* @() */
	  if ('+' == s[1])
	    {
	      multi_trans_line = 1;
	      ++s;
	    }
	}
      else
	{
	  warning("unclosed ')' in label");
	  start_lnum = -1;
	  return;
	}
    }
  else
    {
      /* @label */
      s += 6;
      if ('+' == *s)
	{
	  multi_trans_line = 1;
	  ++s;
	}
      while (isspace(*s))
	++s;
      xstrcpy(label_buf, normalize_ws(s));
    }

  if (!innerp)
    {
      curr_block_np = atf_push("xh:p");
      ctr(curr_block_np);
      /* FIXME: This is not subtle enough--some translation paras
	 will be so long that they should be the #-target
	 themselves, not an earlier para */
      if (next_trans_p_id > 3)
	{
	  sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id-3);
	  atf_xprop(curr_block_np,"xtr:cid",trans_p_idbuf);
	}
      sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id++);
      atf_xprop(curr_block_np,"xml:id",trans_p_idbuf);
      trans_wid = 0;
      labeled_labels(curr_block_np,label_buf);
      se_label(curr_block_np,cc(atfp->name),cc(label_buf));
      if (trans_parenned_labels)
	parenify((char*)label_buf);
      atf_xprop(curr_block_np, "xtr:label",(ccp)label_buf);
      xstrcpy(last_label_buf,label_buf);
    }
}

void
atr_hdr(Mloc l, const char *h, unsigned char *s)
{
  curr_block_np = atf_push(h);
  ctr(curr_block_np);
  if (lth_used == lth_alloced)
    {
      lth_alloced += 8;
      last_trans_h = realloc(last_trans_h, lth_alloced * sizeof(struct node *));
    }
  last_trans_h[lth_used++] = curr_block_np;
  sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id++);
  nocellspan = 1;
  atr_para(l, s);
  nocellspan = 0;
}

void
atr_dollar(Mloc l, unsigned char *s)
{
  struct node *curr_block_np = atf_push("xh:p");
  setClass(curr_block_np,"dollar");
  start_lnum = l.line;
  if (s[1] == '@' && s[2] == '(')
    {
      unsigned char label_buf[128], *lp;
      *label_buf = '\0';
      lp = label_buf;
      s += 3;
      while (*s && *s != ')')
	*lp++ = *s++;
      *lp = '\0';
      if (*s)
	{
	  labeled_labels(curr_block_np,label_buf);
	  atf_xprop(curr_block_np, "xtr:silent","1");
	}
      else
	warning("label on $-line lacks closing ')'");
    }
  else if (dollar_fifo)
    {
      const char *dollar_id = dollar_get();
      if (dollar_id)
	{
	  atf_xprop(curr_block_np,"xtr:ref",dollar_id);
	  if (last_p && *last_xid)
	    {
	      int interval = xid_diff(dollar_id,(const char *)last_xid);
	      if (interval > 1)
		{
		  char buf[10];
		  sprintf(buf,"%d",interval);
		  atf_xprop(last_p,"xtr:sref",getAttr(last_p,"xtr:ref"));
		  removeAttr(last_p, "xtr:ref");
		  atf_xprop(last_p,"xtr:eref",xid_prev(dollar_id));
		  atf_xprop(last_p,"xtr:rows",buf);
		}
	      last_p = NULL;
	    }
	  strcpy((char *)last_xid,(const char *)dollar_id);
	}
      else
	warning("translation dollar-line has no parallel in transliteration");
    }
  else
    {
      atf_xprop(curr_block_np,"xtr:standalone","1");
    }

  ++s;
  while (isspace(*s))
    ++s;

  (void)atr_inline(curr_block_np,s,NULL,0);
}

void
atr_line(Mloc l, const char *s)
{
  Mloc *m = mloc_mloc(&l);
  m->user = (void*)s;
  if (!tral)
    tral = list_create(LIST_SINGLE);
  list_add(tral, m);
}

void
atr_para(Mloc l, unsigned char *s)
{
  static Node *p;
  int with_id = 1;

  const char *label = (ccp)curr_line_label; /* was an arg; is it always set in oxx ? */
  
  if (s && strlen((ccp)s))
    atr_line(l, (ccp)s);

  /* emulate oxx lines[] array */
  int nlines = list_len(tral);
  Mloc **mpp = (Mloc **)list2array(tral);

  /* keep tral cleaned up */
  list_free(tral, NULL);
  tral = NULL;

  unsigned char **lines = malloc((1+nlines)*sizeof(unsigned char *));
  /* When para is a single string, the index into the para can be used
     to find the line num of a token for error messages */
  int *mloc_index = malloc((1+nlines)*sizeof(int));
  int i, nchars;
  for (nchars = i = 0; i < nlines; ++i)
    {
      lines[i] = mpp[i]->user;
      mloc_index[i] = nchars;
      nchars += strlen((ccp)lines[i]);
    }
  s = lines[0];
  unsigned char **start = lines,*sol = s,*text,*first_sol = s;
  int is_comment = 0, spanall = 0;
  start_lnum = mpp[0]->line;

  if (p == NULL)
    p = current_trans->tree;
  if (s == lines[0] && *lines && s[0] == '#')
    is_comment = 1;
  if (*s == '|' && isspace(s[1]))
    {
      spanall = 1;
      *s = ' ';
      s += 2;
    }

  for (i = 0; lines[i]; ++i)
    {
      nchars += xxstrlen(lines[i]);
      ++nchars;
    }

  text = malloc(nchars+1);
  *text = '\0';

  for (i = 0; lines[i]; ++i)
    {
      xstrcat(text,lines[i]);
      xstrcat(text,"\n");
    }
  if (xxstrlen(text) > 0)
    text[xxstrlen(text)-1] = '\0';

  int p_elem;
  if (!strcmp(abt->curr->name, "xh:p"))
    p_elem = 2;
  else
    p_elem = 1;
  
  if (p_elem)
    {
      Node *p = atf_push(p_elem == 2 ? "xh:innerp" : "xh:p");
      if (spanall)
	atf_xprop(p,"xtr:spanall","1");
      if (with_id)
	{
	  sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id++);
	  atf_xprop(p,"xml:id",trans_p_idbuf);
	  trans_wid = 0;
	  /* non-NULL label means caller is trans_inter;
	     this is used for interlinear and parallel;
	     only interlinear can use line_id_buf */
	  if (label)
	    {
	      atf_xprop(p,"xtr:label",label);
	      se_label(p,cc(atfp->name),cc(label));
	      if (current_trans->etype == etu_interlinear)
		{
		  if (tr_id_buf)
		    atf_xprop(p,"xtr:ref",tr_id_buf);
		  else
		    atf_xprop(p,"xtr:ref",line_id_buf);

		  h_arefs(ucc(line_id_buf));
		}
	      else
		{
		  const char *xid = (ccp)check_label((uccp)label,etu_parallel,NULL);

		  if (xid)
		    {
		      atf_xprop(p,"xtr:ref",xid);
		      h_arefs(ucc(xid));
		    }
		  else
		    {
		      vwarning("%s: label used in parallel translation is not in transliteration",
			       label);
		    }
		}
	    }
	}
  
  if (is_comment)
    atf_xprop(p,"class","tr-comment");
  else if (p_elem != 2)
    ctr(p);
  
  if (is_comment)
    atf_add(p,"xh:comment", text); /* FIXME */

  else if (strstr((const char *)text,"@&"))
    {
      int cols_this_para = 0;
      int init_cell = 0;

      while (isspace(*text))
	++text;

      if (text[0] == '@' && text[1] == '&')
	text += 2;

      init_cell = 1;

      /* at the start of this loop, text points at either the span 
	 digits following @& or at the text to process after @& */
      while (*text)
	{
	  unsigned char *c = text, *resume;
	  struct node *cc = atf_add("xh:span");
	  if (!in_note)
	    {
	      setClass(cc,"cell");
	      if (need_dir_rtl)
		atf_xprop(cc,"dir","rtl");
	      atf_xprop(cc,"xtr:span","1");
	    }
	  if (init_cell)
	    {
	      char spanbuf[10];
	      int i = 0;
	      while (isdigit(*c))
		{
		  if (i < 9)
		    spanbuf[i++] = *c++;
		  else
		    {
		      warning("translation cell span too long (max 999999999");
		      break;
		    }
		}
	      spanbuf[i] = '\0';
	      if (!*spanbuf) /* FIXME: @column in parallel trans */
		strcpy(spanbuf,"1");
	      atf_xprop(cc,"xtr:span",spanbuf);
	      cols_this_para += atoi(spanbuf);
	    }
	  
	  while (*text && ('@' != *text || '&' != text[1]))
	    ++text;
	  resume = text;
	  if (*resume)
	    init_cell = 1;

	  else
	    init_cell = 0;
	  while (isspace(text[-1]))
	    --text;
	  *text = '\0';
	  
	  (void)atr_inline(cc,c,NULL,1);
	  text = resume;
	  if (init_cell)
	    text += 2;
	}
      if (cols_this_para > max_trans_cols)
	max_trans_cols = cols_this_para;
    }
  else
    {
      struct node *cc = p;
      while (isspace(*text))
	++text;
      if (*text)
	{
	  if (!nocellspan)
	    {
	      cc = atf_push("xh:span");
	      setClass(cc,"cell");
	      if (need_dir_rtl)
		atf_xprop(cc, "dir", "rtl");
	      atf_xprop(cc,"xtr:span","1");
	    }
	  (void)atr_inline(cc,text,NULL,1);
	}
      else
	{
	  /* unwind the spurious innerp node */
	  p = p->rent;
	  kids_rem_last(p->tree);
	}
    }
    }
}

void
atr_inline(struct node*parent,unsigned char *text,const char *until, int with_trwords)
{
#if 0
  unsigned char *s = text, *start = text;
  int ocurly = 0, nested_curly = 0;

  /*  discretionary(s); now a no-op */

  while (*s)
    {
      if (until && !xstrncmp(s,until,xxstrlen(until)))
	{
	  *s = '\0';
	  s += xxstrlen(until);
	  break;
	}
      else
	{
	  /* We can't preprocess @- blindly because of constructs
	     like @"bow"@-star.  Processing @- here is safe because
	     we are only here at the start of @ sequences */
	  if ('@' == s[0] && '-' == s[1])
	    {
	      s[0] = (unsigned char)0xc2;
	      s[1] = (unsigned char)0xad;
	    }
	  switch (*s)
	    {
	    case '@':
	      *s = '\0';
	      if (s++ > start)
		{
		  if (with_trwords)
		    trwords(parent,start);
		  else
		    textwords(parent,start,s);
		}
	      start = s;
	      if (*s == '"')
		{
		  struct node *span = appendChild(parent,elem(e_xh_span,
							      NULL,lnum,FIELD));
		  setClass(span,"literal");
		  ++s;
		  start = s = trans_inline(span,s,"\"@",1);
		}
	      else if (*s == '?')
		{
		  struct node *span = appendChild(parent,elem(e_xh_span,
							      NULL,lnum,FIELD));
		  setClass(span,"uncertain");
		  ++s;
		  start = s = trans_inline(span,s,"?@",1);
		}
#if 0 /* @- now means discretionary hyphen */
	      else if (*s == '-' || *s == '+')
		{
		  struct node *span = appendChild(parent,elem(e_xh_span
							      ,NULL,lnum,FIELD));
		  if (s[1] != '{')
		    warning("@- and @+ must be followed by '{'");
		  setClass(span,*s == '-' ? "smaller" : "larger");
		  parent = span;
		  start = s+2;
		  ++ocurly;
		}
#endif
	      else if (*s == '&')
		{
		  /* silently ignore @& until we implement it */
		  while (isspace(s[1]))
		    ++s;
		  start = ++s;
		}
	      else
		{
		  while (*s > 127 || isalnum(*s) || '.' == *s 
			 || ('(' == *s || ')' == *s)
			 || ('@' == *s && '-' == s[1]))
		    {
		      if ('@' == *s)
			{
			  s[0] = (unsigned char)0xc2;
			  s[1] = (unsigned char)0xad;
			  s += 2;
			}
		      else
			++s;
		    }
		  if (s > start)
		    {
		      if ('{' == *s)
			{
			  struct node *span = appendChild(parent,elem(e_xh_span,
								      NULL,lnum,FIELD));
			  *s = '\0';
			  setClass(span,start);
			  start = ++s;
			  parent = span;
			  ++ocurly;
			}
		      else
			{
			  unsigned char save = *s;
			  struct node *span = appendChild(parent,elem(e_xh_span,
								      NULL,lnum,FIELD));
			  *s = '\0';
			  appendChild(span,textNode(start));
			  if (all_upper((char *)start))
			    setClass(span,"sign");
			  else
			    setClass(span,"foreign");
			  
			  *s = save;
			  start = s;
			}
		    }
		  else
		    {
		      warning("unexpected character after '@'");
		      start = s;
		    }
		}
	      break;
	    case '{':
	      ++nested_curly;
	      ++s;
	      break;
	    case '}':
	      if (nested_curly > 0)
		{
		  --nested_curly;
		  ++s;
		}
	      else if (ocurly)
		{
		  *s++ = '\0';
		  if (s > start)
		    {
		      const char *class = cc(getAttr(parent,"class"));
		      if (!strcmp(class,"akk")
			  || !strcmp(class,"gdl")
			  || !strcmp(class,"sum"))
			{
			  struct node *res = NULL;
			  unsigned char *buf = NULL;
			  if (strcmp(class, "gdl"))
			    {
			      buf = malloc(strlen(cc(start)) + 6);
			      sprintf((char*)buf,"%%%s %s",class,start);
			      res = gdl(buf, GDL_FRAG_OK);
			      removeAttr(parent,"class");
			      setClass(parent,"gdl");
			    }
			  else
			    res = gdl(start, GDL_FRAG_OK);
			  if (res)
			    {
			      int i;
			      for (i = 0; i < res->children.lastused; ++i)
				appendChild(parent,res->children.nodes[i]);
			    }
			}
		      else if (with_trwords)
			trwords(parent,start);
		      else
			textwords(parent,start,s);
		    }
		  start = s;
		  parent = parent->parent;
		  --ocurly;
		}
	      else
		vwarning("%s: } with no matching {",s++);
	      break;
	    case '^':
	      *s = '\0';
	      if (s++ > start)
		{
		  if (with_trwords)
		    trwords(parent,start);
		  else
		    textwords(parent,start,s);
		  start = s;
		}
	      start = s;
	      while (isalnum(*s))
		++s;
	      if (s > start && '^' == *s)
		{
		  struct node *cnode = NULL;
		  struct node *span = NULL;
		  *s = '\0';
		  cnode = (parent->parent ? parent->parent : parent);
		  if (!strcmp(cc(getAttr(cnode, "class")), "note"))
		    {
		      const unsigned char *nauto = getAttr(cnode,"note:auto");
		      span = appendChild(parent,elem(e_xh_span,NULL,lnum,FIELD));
		      if (nauto && *nauto)
			appendChild(span,textNode(nauto));
		      else
			appendChild(span,textNode(start));
		      setClass(span,"notemark");
		    }
		  else
		    {
		      span = appendChild(parent,elem(e_xh_span,NULL,lnum,FIELD));
		      note_register_tag(start, span);
		      setClass(span,"notelink");
		    }
		  start = ++s;
		}
	      else
		{
		  warning("bad note in ^...^");
		  start = ++s;
		}
	      break;
	    case '#':
	    case '$':
	      if (s > text && s[-1] == '\0')
		goto stop;
	      ++s;
	      break;
	    default:
	      ++s;
	      break;
	    }
	}
    }
 stop:
  if (s > start)
    {
      if (with_trwords)
	trwords(parent,start);
      else
	textwords(parent,start,s);
    }
  return s;
#endif
}

static void
h_arefs(const unsigned char *aref)
{
  int i;
  for (i = 0; i < lth_used; ++i)
    {
      const unsigned char *tlit_h = nth_tlit_hdr(aref,i);
      if (tlit_h)
	appendAttr(last_trans_h[i],attr(a_xtr_hdr_ref,tlit_h));
      else
	appendAttr(last_trans_h[i],attr(a_xtr_ref,aref));
    }
  lth_used = 0;
}

static void
se_label(struct node *np, const char *n, const char *l)
{
  char buf[1024];
  sprintf(buf,"%s, %s", n, l);
  atf_xprop(np,"xtr:se_label",pool_copy((uccp)buf, atfmp->pool));
}

void
set_tr_id(const char *id)
{
  if (id)
    {
      tr_id_buf = realloc(tr_id_buf, strlen(id) + 1);
      strcpy(tr_id_buf, id);
    }
  else
    {
      if (tr_id_buf)
	{
	  free(tr_id_buf);
	  tr_id_buf = NULL;
	}
    }
}

static const char *
xid_prev(const char *x)
{
  static char idbuf[128], *idbufp;
  strcpy(idbuf,x);
  idbufp = idbuf+strlen(idbuf);
  while (idbufp > idbuf && idbufp[-1] != '.')
    --idbufp;
  if (idbufp != idbuf)
    sprintf(idbufp,"%d",atoi(idbufp)-1);
  return idbuf;
}

static void
labeled_labels(struct node *p, unsigned char *lab)
{
  unsigned char *s = lab, *disp = lab, *eref = NULL;
  const unsigned char *xid = NULL;
  int sref = 0, overlap = 0;
  unsigned const char *sref_xid = NULL;
  int saved_start_lnum = start_lnum;
  
  while (*disp)
    {
      if (isspace(*disp) && disp[1] == '=' && isspace(disp[2]))
	break;
      else
	++disp;
    }
  if (*disp)
    {
      unsigned char *end = disp, save = 0;
      while (end > lab && isspace(end[-1]))
	--end;
      *end = '\0';
      disp += 3;
      while (isspace(*disp))
	++disp;
      end = disp;
      while (*end && !isspace(*end))
	++end;
      if (isspace(*end))
	{
	  save = *end;
	  *end = '\0';
	}
      setAttr(p,a_xtr_lab_start_label,pool_copy(disp));
      setAttr(p,a_xtr_lab_start_lnum,pool_copy(lnum_of(disp)));
      if (save)
	*end = save;
      s = end;
    }

  while (*s && (*s != '-' || (s == lab || !isspace(s[-1]) || !isspace(s[1]))))
    ++s;

  if (*s)
    {
      eref = s+1;
      while (isspace(s[-1]))
	--s;
      *s = '\0';
    }

  start_lnum = p->lnum;
  xid = check_label(lab,etu_labeled,NULL);
  start_lnum = saved_start_lnum;

  if (xid)
    {
      sref_xid = xid;
      sref = int_of(xid);
      if (*last_xid && !strcmp((const char*)last_xid, (const char *)xid))
	overlap = 1;
    repeat:
      if (dollar_fifo)
	{
	  /* trap places where a translation line occurs but the
	     next corresponding item in the transliteration is a
	     $-line.  FIXME: what if the translation has a labeled
	     $-line so the translation line matches up to it--is that
	     taken care of by not including labeled dollar lines in the
	     dollar fifo?		 
	  */
	  const char *doll_id = dollar_peek();
	  if (doll_id)
	    {
	      /* doll_id should be later than the trans line */
	      int interval = xid_diff((const char *)xid,doll_id);
	      if (interval > 0)
		{
		  vwarning2(file,p->lnum,"expected $-line to match transliteration.\n\tTo have no corresponding $-line include a spacer in the translation:\n\t\t$ (SPACER)");
		  /* flush the translit dollar line that has no
		     counterpart in the translat */
		  (void)dollar_get();
		  goto repeat; /* trap multiple missing SPACERs */
		}
	    }
	}
      if (last_p && *last_xid)
	{
	  /* automatically provide sref/eref even when user only gives
	     sref */
	  int interval = xid_diff((const char*)xid,(const char *)last_xid);
	  if (interval > 1 || (interval==0 && multi_trans_line))
	    {
	      char buf[10];
	      sprintf(buf,"%d",interval);
	      setAttr(last_p,a_xtr_sref,getAttr(last_p,"xtr:ref"));
	      removeAttr(last_p, "xtr:ref");
	      setAttr(last_p,a_xtr_eref,xid_prev((const char *)xid));
	      setAttr(last_p,a_xtr_rows,buf);
	    }
	  else if (interval < 1)
	    {
	      vwarning2(file,p->lnum,"translation alignment out of order");
	    }
	}
      else if (*last_xid && dollar_fifo)
	{
	  /* last item was a trans_dollar -- trap backtracking */
	  int interval = xid_diff((const char*)xid,(const char *)last_xid);
	  if (interval < 0)
	    vwarning2(file,p->lnum,"preceding $-line incorrectly aligned");
	}

      if (eref)
	setAttr(p,a_xtr_sref,xid);
      else
	setAttr(p,a_xtr_ref,xid);
      if (!*getAttr(p,"xtr:lab-start-label"))
	{
	  setAttr(p,a_xtr_lab_start_label,pool_copy(lab));
	  setAttr(p,a_xtr_lab_start_lnum,pool_copy(lnum_of(lab)));
	}
      if (overlap)
	setAttr(p,a_xtr_overlap,(unsigned const char *)"1");
      strcpy((char *)last_xid,(const char *)xid);
      last_p = p;
      h_arefs(xid);
    }
  else
    {
      *last_xid = '\0';
      last_p = NULL;
    }
  
  if (eref)
    {
      s = eref;
      while (isspace(*s))
	++s;
      xid = check_label(s,etu_labeled,NULL);
      if (xid)
	{
	  char buf[5];
	  int interval = xid_diff((const char*)xid,(const char *)sref_xid);
	  if (interval < 1)
	    {
	      vwarning2(file,p->lnum,"end of range comes before start of range");
	      /* keep processing even though it will produce unsightly output */
	    }
	  else
	    strcpy((char *)last_xid,(const char *)xid);
	  sprintf(buf,"%d",1 + int_of(xid) - sref);
	  setAttr(p,a_xtr_eref,xid);
	  setAttr(p,a_xtr_lab_end_label,pool_copy(s));
	  setAttr(p,a_xtr_lab_end_lnum,pool_copy(lnum_of(s)));
	  setAttr(p,a_xtr_rows,ucc(buf));
	  /* This attribute means that the start of this trans block
	     overlaps with the end of the preceding one */
	  if (overlap)
	    setAttr(p,a_xtr_overlap,(unsigned const char *)"1");
	}
      else
	*last_xid = '\0';
      last_p = NULL;
    }

  if (trans_abbrevved_labels && *last_label_buf && xstrcmp(getClass(p),"dollar"))
    {
      unsigned char *prefix = label_prefix(lab);
      if (prefix && !strncmp(cc(last_label_buf),cc(prefix),strlen(cc(prefix))))
	{
	  unsigned char *rendlabel = lab + strlen(cc(prefix));
	  while (isspace(*rendlabel))
	    ++rendlabel;
	  setAttr(p,a_xtr_rend_label,rendlabel);
	}
    }
}

static void
parenify(char *buf)
{
  char *tmp = malloc(strlen(buf)+3);
  if (!tmp)
    {
      fputs("atf2xtf: out of core\n",stderr);
      exit(2);
    }
  strcpy(tmp,"(");
  strcat(tmp,buf);
  strcat(tmp,")");
  strcpy(buf,tmp);
  free(tmp);
}

