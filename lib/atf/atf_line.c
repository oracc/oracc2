#include <oraccsys.h>
#include "atf.h"
#include "atf_bld.h"
#include "otf-defs.h"

void
line_mts(unsigned char *lp)
{
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
  struct attr *ap,*xid;
  unsigned char *tok = lp;
  unsigned char *end = lp+xxstrlen(lp);
  unsigned char *s = lp;

  ++lninfo.lineno;

  already_lemmed = 0;
  bil_offset = exemplar_offset = 0;
  curr_lang_ctxt = text_lang;
  protocol_state = s_intra;

  while (*s && !isspace(*s))
    {
      ++s;
    }
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
      set_tr_id(line_id_buf);
    }
  xid = attr(a_xml_id,ucc(line_id_buf));
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
	  hash_add(last_tlit_h_hash,(const unsigned char *)xid->valpair[1],tmp);
	}
      lth_used = 0;
    }
  appendAttr(lnode,xid);
  if (atf_cbd_err)
    {
      extern int cbd_err_line;
      cbd_err_line = atoi((char*)tok);
    }
  if (!mylines)
    {
      appendAttr(lnode,attr(a_o,tok));
      tok = (unsigned char*)lnstr(lninfo.lineno,lninfo.lineprimes);
      appendAttr(lnode,attr(a_n,tok));
    }
  else
    appendAttr(lnode,attr(a_n,tok));

  curr_line_label = line_label(tok,0,(unsigned const char *)xid->valpair[1]);
  if (curr_line_label)
    {
      ap = attr(a_label,curr_line_label);
      appendAttr(lnode,ap);
      extern char *label2;
      if (label2)
	{
	  ap = attr(a_label2,(unsigned const char *)label2);
	  appendAttr(lnode,ap);
	}
      register_label(labtab,uc(xid->valpair[1]),uc(ap->valpair[1]));
      free((char*)curr_line_label);
      curr_line_label = ucc(ap->valpair[1]);
    }
  appendChild(current,lnode);
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      /* a leading |<SPACE> means span all cells */
      if (*s == '|' && isspace(s[1]))
	{
	  s += 2;
	  appendAttr(lnode,attr(a_spanall,(unsigned char *)"1"));
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
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
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
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
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
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
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
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
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
  struct node *lnode = elem(e_v,NULL,lnum,LINE);
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
