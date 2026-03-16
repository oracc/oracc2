#include <oraccsys.h>
#include "atf.h"

#if 1
#define ctr(p) atfxprop((p),"class","tr")
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
	      xstrcpy(label_buf,normalize_ws(t)));
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
      curr_block = atr_push("xh:p");
#if 0
      curr_block = appendChild(text,
			       elem(e_xh_p,NULL,lnum,DIVISION));
      curr_block->lnum = start_lnum;
#endif
      ctr(curr_block);
      /* FIXME: This is not subtle enough--some translation paras
	 will be so long that they should be the #-target
	 themselves, not an earlier para */
      if (next_trans_p_id > 3)
	{
	  sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id-3);
	  atf_xprop(curr_block,"xtr:cid",trans_p_idbuf);
	}
      sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id++);
      atf_xprop(curr_block,"xml:id",trans_p_idbuf);
      trans_wid = 0;
      labeled_labels(curr_block,label_buf);
      se_label(curr_block,cc(text_n),cc(label_buf));
      if (trans_parenned_labels)
	parenify((char*)label_buf);
      atf_xprop(curr_block, "xtr:label",label_buf);
      xstrcpy(last_label_buf,label_buf);
    }
}

void
atr_hdr(Mloc l, unsigned char *s)
{
  curr_block = atf_push("h");
  ctr(curr_block);
  if (lth_used == lth_alloced)
    {
      lth_alloced += 8;
      last_trans_h = realloc(last_trans_h, lth_alloced * sizeof(struct node *));
    }
  last_trans_h[lth_used++] = curr_block;
  sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id++);
  nocellspan = 1;
  atr_para(s);
  nocellspan = 0;
}

void
atr_dollar(Mloc l, unsigned char **lines)
{
  unsigned char *s = s;
  struct node *curr_block = appendChild(current_trans->tree,
					elem(e_xh_p,NULL,lnum,DIVISION));
  setClass(curr_block,"dollar");
  start_lnum = lnum;
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
	  labeled_labels(curr_block,label_buf);
	  appendAttr(curr_block, attr(a_xtr_silent,ucc("1")));
	}
      else
	warning("label on $-line lacks closing ')'");
    }
  else if (dollar_fifo)
    {
      const char *dollar_id = dollar_get();
      if (dollar_id)
	{
	  setAttr(curr_block,a_xtr_ref,(unsigned char *)dollar_id);
	  if (last_p && *last_xid)
	    {
	      int interval = xid_diff(dollar_id,(const char *)last_xid);
	      if (interval > 1)
		{
		  char buf[10];
		  sprintf(buf,"%d",interval);
		  setAttr(last_p,a_xtr_sref,getAttr(last_p,"xtr:ref"));
		  removeAttr(last_p, "xtr:ref");
		  setAttr(last_p,a_xtr_eref,(unsigned char *)xid_prev(dollar_id));
		  setAttr(last_p,a_xtr_rows,(unsigned char *)buf);
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
      setAttr(curr_block,a_xtr_standalone,(unsigned char *)"1");
    }

#if 0 /* $ does not work like headings */
  if (lth_used == lth_alloced)
    {
      lth_alloced += 8;
      last_trans_h = realloc(last_trans_h, lth_alloced * sizeof(struct node *));
    }
  last_trans_h[lth_used++] = curr_block;
#endif

  ++s;
  while (isspace(*s))
    ++s;
  (void)trans_inline(curr_block,s,NULL,0);

  /* start_lnum = -1; */

  return lines;
}

void
atr_para(unsigned char **lines, unsigned char *s, struct node *p, int p_elem, 
	   int with_id, unsigned const char *label, int stop_at_lnum)
{
  int nchars = 0;
  unsigned char **start = lines,*sol = s,*text,*first_sol = s;
  int is_comment = 0, spanall = 0;
  start_lnum = lnum;
  if (p == NULL)
    p = current_trans->tree;
  if (s == s && *lines && s[0] == '#')
    is_comment = 1;
  if (*s == '|' && isspace(s[1]))
    {
      spanall = 1;
      *s = ' ';
      s += 2;
    }
  while (*lines)
    {
      if (lnum > start_lnum && !isspace(*s))
	{
	  if (stop_at_lnum
	      && (current_trans->etype == etu_interlinear 
		  || current_trans->etype == etu_parallel))
	    break; /* break at non-space sol after first line if parallel/interlinear */
	  else if (*s == '&' || *s == '$' 
		   || (*s == '#' && !is_comment))
	    {
	      notice("possible failure to end translation para with blank line (found `&' or '$')");
	      goto stop;
	    }
	  else if (!stop_at_lnum && looks_like_lnum(s))
	    {
	      notice("possible failure to end translation para with blank line");
	      goto stop;
	    }
	  else if (*s == '@')
	    {
	      /* break on a block token; keep processing otherwise */
	      unsigned char *t = s+1, *e = s+1;
	      char save;
	      while (*e && !isspace(*e))
		++e;
	      save = *e;
	      *e = '\0';
	      if (blocktok((const char *)t,e-t))
		{
		  *e = save;
		  break;
		}
	      else
		*e = save;
	      
	    }
	}
      while (isspace(*s))
	++s;
      if (!*s)
	break; /* always break at a blank link */
      else
	{
	  if (need_lemm && sol == s)
	    lem_save_line((const char *)sol);
	  nchars += xxstrlen(sol);
	  ++nchars;
	  ++lines;
	  ++lnum;
	  s = sol = *lines;
	}
    }
  text = malloc(nchars+1);
  *text = '\0';
  s = first_sol;
  while (start < lines)
    {
      xstrcat(text,s);
      xstrcat(text,"\n");
      ++start;
      s = *start;
    }
  if (xxstrlen(text) > 0)
    text[xxstrlen(text)-1] = '\0';
  if (p_elem)
    {
      p = appendChild(p,elem(p_elem == 2 ? e_xh_innerp : e_xh_p,NULL,lnum,DIVISION));
      if (spanall)
	appendAttr(p,attr(a_xtr_spanall,(unsigned char *)"1"));
      if (with_id)
	{
	  sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id++);
	  appendAttr(p,attr(a_xml_id,ucc(trans_p_idbuf)));
	  trans_wid = 0;
	  /* non-NULL label means caller is trans_inter;
	     this is used for interlinear and parallel;
	     only interlinear can use line_id_buf */
	  if (label)
	    {
	      appendAttr(p,attr(a_xtr_label,label));
	      se_label(p,cc(text_n),cc(label));
	      if (current_trans->etype == etu_interlinear)
		{
		  if (tr_id_buf)
		    appendAttr(p,attr(a_xtr_ref,ucc(tr_id_buf)));
		  else
		    appendAttr(p,attr(a_xtr_ref,ucc(line_id_buf)));

		  setAttr(p,n_xh,(unsigned char *)nstab[n_xh].ns);
		  setAttr(p,n_xtr,(unsigned char *)nstab[n_xtr].ns);

		  h_arefs(ucc(line_id_buf));
		}
	      else
		{
		  const unsigned char *xid = check_label(label,etu_parallel,NULL);

		  setAttr(p,n_xh,(unsigned char *)nstab[n_xh].ns);
		  setAttr(p,n_xtr,(unsigned char *)nstab[n_xtr].ns);

		  if (xid)
		    {
		      appendAttr(p,attr(a_xtr_ref,ucc(xid)));
		      h_arefs(ucc(xid));
		    }
		  else
		    {
		      vwarning("%s: label used in parallel translation is not in transliteration", label);
		    }
		}
	    }
	}
      if (is_comment)
	setAttr(p,a_class,ucc("tr-comment"));
      else if (p_elem != 2)
	ctr(p);
    }
  else if (label && last_trans_h)
    h_arefs(ucc(line_id_buf));

  if (is_comment)
    appendChild(p,textNode(text));
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
	  struct node *cc = appendChild(p,elem(e_xh_span,NULL,lnum,CELL));
	  if (!in_note)
	    {
	      setClass(cc,"cell");
	      if (need_dir_rtl)
		appendAttr(cc,attr(a_dir,(uccp)"rtl"));
	      appendAttr(cc,attr(a_xtr_span,ucc("1")));
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
	      setAttr(cc,a_xtr_span,ucc(spanbuf));
	      cols_this_para += atoi(spanbuf);
	    }

	  while (*text && ('@' != *text || '&' != text[1]))
	    ++text;
	  resume = text;
#if 1
	  if (*resume)
	    init_cell = 1;

	  else
	    init_cell = 0;
#else
	  if (*resume)
	    {
	      resume += 2; /* skip @& */
	      ++cols_this_para;
	      if (isspace(*resume))
		{
		  while (isspace(*resume))
		    ++resume;
		  init_cell = 0;
		}
	      else
		init_cell = 1;
	    }
#endif
	  while (isspace(text[-1]))
	    --text;
	  *text = '\0';
	  
	  (void)trans_inline(cc,c,NULL,1);
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
	      cc = appendChild(p,elem(e_xh_span,NULL,lnum,CELL));
	      setClass(cc,"cell");
	      if (need_dir_rtl)
		appendAttr(cc,attr(a_dir,(uccp)"rtl"));
	      appendAttr(cc,attr(a_xtr_span,ucc("1")));
	    }
	  (void)trans_inline(cc,text,NULL,1);
	}
      else
	{
	  /* unwind the spurious innerp node */
	  p = p->parent;
	  removeLastChild(p);
	}
    }
 stop:
  if (lnum == start_lnum)
    {
      ++lnum;
      ++lines;
    }

  /*start_lnum = -1;*/
  
  return lines;
}

void
atr_inline(struct node*parent,unsigned char *text,const char *until, int with_trwords)
{
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
}
