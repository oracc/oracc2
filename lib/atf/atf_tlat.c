#include <oraccsys.h>
#include <mesg.h>
#include <lang.h>
#include <inl.h>
#include "atf.h"
#include "atf_bld.h"
#include "otf-defs.h"

extern int bld_trace;

struct translation **ax_translations = NULL;
struct translation *curr_trans;

static int multi_trans_line;
static char trans_p_idbuf[128];
static int trans_wid;
static Prop *p;
static Hash *last_tlit_h_hash = NULL;
int max_trans_cols = 1;
int need_alignment = 1;
int nocellspan = 0;
int trans_parenned_labels = 1;
int trans_abbrevved_labels = 0;
static char trans_id_base[32];
static unsigned char last_xid[128];
extern int start_lnum;
extern int next_trans_id;
static int next_trans_p_id;
extern int saa_mode;
extern unsigned const char *curr_line_label;
static unsigned char label_buf[128];
static unsigned char last_label_buf[128];

static Node *last_trans_node;

static int in_note = 0/*, last_label = 0*/;
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
static const unsigned char *nth_tlit_hdr(const unsigned char *id, int nth);
static int int_of(unsigned const char *line_id);
static unsigned char *label_prefix(unsigned char *lab);
static unsigned char *lnum_of(unsigned char *l);

static void atr_inline(Mloc *mp, struct node*parent,unsigned char *text);

static List *tral;

#if 1
#define ctr(p) atf_xprop((p),"class","tr")
#define setClass(n,c) atf_xprop((n),"class",c)
#define getAttr(n,a) ((p=prop_find_kv(n->props, a, NULL))?p->u.k->v:NULL)
#define getClass(n) getAttr((n),"class")
#define removeAttr(n,a) prop_drop_kv(n->props, a, NULL)
#define setAttr(n,a,v) atf_xprop((n),(a),(v))
#define appendAttr(n,a,v) atf_xprop((n),(a),(v))
#else
#define xctr(p) if (p && !*(getAttr(p,a_class))) appendAttr((p),attr(a_class,ucc("tr")))
#define yctr(p) appendAttr((p),attr(a_class,ucc("tr")))
#define ctr(p) setAttr((p),a_class,ucc("tr"))
#define setClass(n,c) appendAttr((n),attr(a_class,ucc((c))))
#define getClass(n) getAttr((n),"class")
#endif

Node *
atr_push(const char *s, Mloc *mp)
{
  if (bld_trace)
    fprintf(stderr, "bld: atr_push %s to parent %s\n", s, curr_trans->tree->curr->name);
  Node *np = tree_add(curr_trans->tree, NS_HTM, s, curr_trans->tree->curr->depth, NULL);
  np->mloc = mloc_mloc(mp);
  return tree_push(curr_trans->tree);
}

void
atr_translation(void)
{
  extern int status;
  *last_label_buf = '\0';
  *last_xid = '\0';
  status = 0;
  /*last_label = 0;*/
  trans_wid = 0;

  if (dollar_fifo)
    dollar_reset();

  /* trans_inter does not call the translation() routine, which is good
     because we don't want to reinitialize the line notes in interlinear
     translations */
  note_initialize_line();
  note_index = 1;

  if (saa_mode)
    need_alignment = 0;
  else
    need_alignment = 1;

  curr_trans->id = (uccp)atfp->pqx;
  appendAttr(curr_trans->tree->root,"ref",(ccp)curr_trans->id);
  appendAttr(curr_trans->tree->root,"n", (ccp)atfp->name);
  appendAttr(curr_trans->tree->root,"project", (ccp)atfp->project);
  appendAttr(curr_trans->tree->root,"xml:lang",curr_trans->lang);
  if (langrtl(curr_trans->lang,strlen(curr_trans->lang)))
    {
      need_dir_rtl = 1;
      appendAttr(curr_trans->tree->root,"dir","rtl");
    }
  else
    need_dir_rtl = 0;
  appendAttr(curr_trans->tree->root,"xtr:type",(ccp)curr_trans->type);
  appendAttr(curr_trans->tree->root,"xtr:code",(ccp)curr_trans->code);
  sprintf(trans_id_base,"%s_%s-%s",curr_trans->id,curr_trans->code,curr_trans->lang);
#if 0
  if (trans_hash_add(trans_id_base))
    {
      warning("duplicate @translation will be ignored");
      return;
    }
#endif
  appendAttr(curr_trans->tree->root,"xml:id",(ccp)trans_id_base);
  next_trans_p_id = 0;
#if 0
  (void)refattrs(NULL,NULL,NULL);
  mapentry(NULL,NULL);
#endif

#if 0
  /* Catch the user if labeled translations are being used without mylines */
  if ((tt->type == etu_labeled || tt->type == etu_parallel) && !mylines)
    notice("#atf: use mylines recommended with labeled/parallel translations");
#endif
}

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
    atr_para();

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
      curr_block_np = atr_push("xh:p", &l);
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
  curr_block_np = atr_push(h, &l);
  ctr(curr_block_np);
  if (lth_used == lth_alloced)
    {
      lth_alloced += 8;
      last_trans_h = realloc(last_trans_h, lth_alloced * sizeof(struct node *));
    }
  last_trans_h[lth_used++] = curr_block_np;
  sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id++);
  nocellspan = 1;
  atr_para();
  nocellspan = 0;
}

void
atr_dollar(Mloc l, unsigned char *s)
{
  struct node *curr_block_np = atr_push("xh:p", &l);
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

  (void)atr_inline(&l, curr_block_np,s);
}

void
atr_text(Mloc l, const char *s)
{
  Mloc *m = mloc_mloc(&l);
  m->user = (void*)s;
  if (!tral)
    tral = list_create(LIST_SINGLE);
  list_add(tral, m);
}

/* In the lib/atf architecture the <xh:p> node is set up by atr_label
   and this should only be called when processing a para of
   translation */
void
atr_para(void)
{
  int with_id = 0; /* this may need to be adjusted for interlinear */

  const char *label = (ccp)curr_line_label; /* was an arg; is it always set in oxx ? */
  
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
  unsigned char *s = lines[0];
  unsigned char *text;
  int is_comment = 0, spanall = 0;
  start_lnum = mpp[0]->line;

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
      /*xstrcat(text,"\n");*/
    }
  if (xxstrlen(text) > 0)
    text[xxstrlen(text)-1] = '\0';

  int p_elem;
  if (!strcmp(curr_trans->tree->curr->name, "xh:p"))
    p_elem = 2;
  else
    p_elem = 1;
  
  if (p_elem)
    {
      Node *p = curr_trans->tree->curr;
      /*atr_push(p_elem == 2 ? "xh:innerp" : "xh:p", mpp[0]);*/
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
	      if (curr_trans->etype == etu_interlinear)
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
		      mesg_verr(mpp[0],
				"%s: label used in parallel translation is not in transliteration",
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
    {
      Node *np = atf_add("xh:comment", mpp[0]);
      np->text = (ccp)text;
    }

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
	  struct node *cc = atf_add("xh:span", mpp[0]);
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
	  
	  (void)atr_inline(mpp[0],cc,text);
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
	      cc = atr_push("xh:span", mpp[0]);
	      setClass(cc,"cell");
	      if (need_dir_rtl)
		atf_xprop(cc, "dir", "rtl");
	      atf_xprop(cc,"xtr:span","1");
	    }
	  (void)atr_inline(mpp[0],cc,text);
	}
      else
	{
#if 0
	  /* unwind the spurious innerp node */
	  p = p->rent;
	  kids_rem_last(p->tree);
#endif
	}
    }
    }
}

static void
atr_inline(Mloc *mp, struct node*parent,unsigned char *text)
{
  /*fprintf(stderr, "atr_inline received===\n%s\n", text);*/
  extern int inl_wild_mode;
  inl_wild_mode = 1;
  char *ws = normalize_ws(text);
  Tree *itp = inl(mp, ws);
  tree_graft(parent, itp);
  free(ws);
  inl_wild_mode = 0;
}

static void
h_arefs(const unsigned char *aref)
{
  int i;
  for (i = 0; i < lth_used; ++i)
    {
      const unsigned char *tlit_h = nth_tlit_hdr(aref,i);
      if (tlit_h)
	atf_xprop(last_trans_h[i],"xtr:hdr_ref",(ccp)tlit_h);
      else
	atf_xprop(last_trans_h[i],"xtr:ref",(ccp)aref);
    }
  lth_used = 0;
}

static const unsigned char *
nth_tlit_hdr(const unsigned char *id, int nth)
{
  struct node **tmp_tlit_h = NULL;
  if (id)
    {
      tmp_tlit_h = hash_find(last_tlit_h_hash, id);
      if (tmp_tlit_h)
	{
	  int i;
	  for (i = 0; i < nth; ++i)
	    if (NULL == tmp_tlit_h[i])
	      break;
	  if (tmp_tlit_h[i])
	    return (uccp)getAttr(tmp_tlit_h[i],"xml:id");
	}
    }
  return NULL;
}

static void
se_label(struct node *np, const char *n, const char *l)
{
  char buf[1024];
  sprintf(buf,"%s, %s", n, l);
  atf_xprop(np,"xtr:se_label",(ccp)pool_copy((uccp)buf, atfmp->pool));
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

static int
xid_line(const char *x)
{
  const char *start;
  if (!x)
    return 0;
  start = x;
  x += strlen(x);
  while (x > start && x[-1] != '.')
    --x;
  if (isdigit(*x))
    return atoi(x);
  else
    return 0;
}

static int
xid_vchk(const char *x, int from, int to)
{
  extern Hash *vreg;
  char buf[strlen(x)+1], *bufp;
  int v = 0;
  strcpy(buf, x);
  if ((bufp = strchr(buf, '.')))
    {
      ++bufp;
      while (from < to)
	{
	  sprintf(bufp, "%d", from++);
	  void *vp = hash_find(vreg, (uccp)buf);
	  if (vp)
	    v += (uintptr_t)vp;
	}
    }
  return v;
}

/*WATCHME: this only works perfectly if the line id mechanism
  increments the line id by 1 for each line_mts--the current
  implementation may be good enough for extant uses */
static int
xid_diff(const char *x1, const char *x2)
{
  int i1,i2;
  i1 = xid_line(x1);
  i2 = xid_line(x2);
  int v = xid_vchk(x2, i2, i1);
  return (i1-i2)+v;
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
labeled_labels(struct node *np, unsigned char *lab)
{
  unsigned char *s = lab, *disp = lab, *eref = NULL;
  const unsigned char *xid = NULL;
  int sref = 0, overlap = 0;
  unsigned const char *sref_xid = NULL;
  int saved_start_lnum = start_lnum;

  last_trans_node = np;
  
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
      setAttr(np,"xtr:lab-start-label",(ccp)pool_copy((uccp)disp, atfmp->pool));
      setAttr(np,"xtr:lab-start-lnum",(ccp)pool_copy((uccp)lnum_of(disp), atfmp->pool));
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

  start_lnum = np->mloc->line;
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
		  vwarning2(np->mloc->file,np->mloc->line,
			    "expected $-line to match transliteration.\n"
			    "\tTo have no corresponding $-line include a spacer in the translation:\n"
			    "\t\t$ (SPACER)");
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
	      setAttr(last_p,"xtr:sref",getAttr(last_p,"xtr:ref"));
	      removeAttr(last_p, "xtr:ref");
	      setAttr(last_p,"xtr:eref",xid_prev((const char *)xid));
	      setAttr(last_p,"xtr:rows",buf);
	    }
	  else if (interval < 1)
	    {
	      vwarning2(np->mloc->file,np->mloc->line,"translation alignment out of order");
	    }
	}
      else if (*last_xid && dollar_fifo)
	{
	  /* last item was a trans_dollar -- trap backtracking */
	  int interval = xid_diff((const char*)xid,(const char *)last_xid);
	  if (interval < 0)
	    vwarning2(np->mloc->file,np->mloc->line,"preceding $-line incorrectly aligned");
	}

      if (eref)
	setAttr(np,"xtr:sref",(ccp)xid);
      else
	setAttr(np,"xtr:ref",(ccp)xid);
      const char *lsl = getAttr(np,"xtr:lab-start-label");
      if (!lsl || !*lsl)
	{
	  setAttr(np,"xtr:lab-start-label",(ccp)pool_copy(lab, atfmp->pool));
	  setAttr(np,"xtr:lab-start-lnum",(ccp)pool_copy(lnum_of(lab), atfmp->pool));
	}
      if (overlap)
	setAttr(np,"xtr:overlap","1");
      strcpy((char *)last_xid,(const char *)xid);
      last_p = np;
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
	      vwarning2(np->mloc->file,np->mloc->line,"end of range comes before start of range");
	      /* keep processing even though it will produce unsightly output */
	    }
	  else
	    strcpy((char *)last_xid,(const char *)xid);
	  sprintf(buf,"%d",1 + int_of(xid) - sref);
	  setAttr(np,"xtr:eref",(ccp)xid);
	  setAttr(np,"xtr:lab_end_label",(ccp)pool_copy(s, atfmp->pool));
	  setAttr(np,"xtr:lab_end_lnum",(ccp)pool_copy(lnum_of(s), atfmp->pool));
	  setAttr(np,"xtr:rows",buf);
	  /* This attribute means that the start of this trans block
	     overlaps with the end of the preceding one */
	  if (overlap)
	    setAttr(np,"xtr:overlap","1");
	}
      else
	*last_xid = '\0';
      last_p = NULL;
    }

  if (trans_abbrevved_labels && *last_label_buf && xstrcmp(getClass(np),"dollar"))
    {
      unsigned char *prefix = label_prefix(lab);
      if (prefix && !strncmp(cc(last_label_buf),cc(prefix),strlen(cc(prefix))))
	{
	  unsigned char *rendlabel = lab + strlen(cc(prefix));
	  while (isspace(*rendlabel))
	    ++rendlabel;
	  setAttr(np,"xtr:rend_label",(ccp)rendlabel);
	}
    }
}

void
atr_finish_labels(void)
{
  if (!last_trans_node)
    return;

  if (!*last_xid)
    {
      last_trans_node = NULL;
      return;
    }

  char *xid = (char*)last_xid;
  const char *refid = getAttr(last_trans_node, "xtr:sref");
  if (!refid || !*refid)
    refid = getAttr(last_trans_node, "xtr:ref");
  if (*refid && strcmp(refid,xid))
    {
      char xxid[128], *xp;
      strcpy(xxid, xid);
      xp = strchr(xxid, '.');
      if (xp)
	{
	  ++xp;
	  int xi = atoi(xp);
	  sprintf(xp, "%d", xi+1);
	}
      int interval = xid_diff((const char*)xxid,(const char *)last_xid);
      char buf[10];
      sprintf(buf,"%d",interval);
      removeAttr(last_trans_node,"xtr:ref");
      removeAttr(last_trans_node, "xtr:ref");
      setAttr(last_trans_node,"xtr:sref",refid);
      setAttr(last_trans_node,"xtr:eref",xid);
      setAttr(last_trans_node,"xtr:rows",buf);
    }    
}

static int
int_of(unsigned const char *line_id)
{
  unsigned const char *s = line_id + xxstrlen(line_id);
  while (s > line_id && s[-1] != '.')
    --s;
  return atoi(cc(s));
}

static unsigned char *
label_prefix(unsigned char *lab)
{
  static unsigned char buf[128], *b;
  xstrcpy(buf,lab);
  b = buf+strlen(cc(buf));
  while (b > buf && !isspace(b[-1]))
    --b;
  if (b > buf)
    {
      while (isspace(b[-1]))
	--b;
      b[1] = '\0';
      return buf;
    }
  else
    return NULL;
}

static unsigned char *
lnum_of(unsigned char *l)
{
  static unsigned char buf[128];
  unsigned char *l_end;
  l_end = l+strlen((char*)l);
  while (l_end > l && !isspace(l_end[-1]))
    --l_end;
  strcpy((char*)buf,(char*)l_end);
  return buf;
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

