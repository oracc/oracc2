#include <oraccsys.h>
#include <setjmp.h>
#include <runexpat.h>
#include <xml.h>
#include "cfy.h"

int in_l = 0, inner = 0;
int protocol_cfy_ccf;
char innertags[128];

Elt e_zwj = { .etype=ELT_J, .data=(ucp)"\xE2\x80\x8D" };
Elt e_zwnj = { .etype=ELT_N, .data=(ucp)"\xE2\x80\x8C" };
Elt e_zws = { .etype=ELT_S, .data=(ucp)"\xE2\x80\x8B" };

int zws_pending = 0, zwnj_pending = 0, plus_pending = 0;

static Elt *last_ep = NULL;
static Line *curr_line;
static int prev_last_w; /* we keep the prev_last_w to use when we pop
			   the final w of the line off the list */

static Btype
breakage(const char *name, const char **atts)
{
  const char *utf8 = findAttr(atts, "g:utf8");
  const char *b = findAttr(atts, "g:break");
  if (!strcmp(b, "missing"))
    return BRK_LOST;
  else if (!strcmp(b, "damaged") || 'x' == *utf8 || 'X' == *utf8)
    return BRK_HASH;
  return BRK_NONE;
}

static void
cfy_cell(Cfy *c, const char **atts)
{
  Elt *ep = memo_new(c->m_elt);
  ep->etype = ELT_C;
  ep->prev = last_ep;
  last_ep = ep;
  Cell *cellp = memo_new(c->m_cell);
  const char *span = findAttr(atts, "span");
  if (span)
    cellp->span = atoi(span);
  else
    cellp->span = 1;
  ep->data = cellp;
  list_add(c->line, ep);
}

static void
cfy_grapheme(Cfy *c, const char *name, const char **atts, const char *utf8, Class *cp, const char *oid)
{
  static int zwj_murub = 0;
  
  if (c->coverage)
    cfy_uni_check(c, (uccp)utf8);

  Btype brk = breakage(name, atts); /* front this because zwj needs it */

  if (zwj_murub)
    {
      char *d = last_ep->data;
      d += strlen(d)-3;
      if (ELT_G == last_ep->etype && !strcmp(d, e_zwj.data))
	{
	  char buf[strlen((char*)last_ep->data)+strlen(utf8)+1];
	  strcpy(buf, (char*)last_ep->data);
	  strcat(buf, utf8);
	  last_ep->data = hpool_copy((uccp)buf, c->hp);
	  /* emulate cfy_liga_breakage where differing adjacent btypes all map to BRK_HASH */
	  if (brk != last_ep->btype)
	    last_ep->btype = BRK_HASH;
	  /* we are done; the new grapheme has been concatenated onto the prev+ZWJ data */
	  return;
	}
      else
	fprintf(stderr, "%s:%d: discontinuous ZWJ (sign'+'nonsign)\n", pi_file, pi_line);
      zwj_murub = 0;
    }    
  
  /* can't currently do G_U because ATF $MU doesn't annotate the $-status*/
  Gtype g = (name[2] == 'v'
	     ? G_V : (name[2] == 's'
		      ? G_S : (name[2] == 'n'
			       ? G_N : G_X)));

  size_t wlen;
  (void)utf2wcs((uccp)utf8,&wlen);
  if (wlen > 1)
    g = G_Q;
  
  if (!c->line)
    c->line = list_create(LIST_DOUBLE);

  Elt *ep = memo_new(c->m_elt);
  ep->prev = last_ep;
  last_ep = ep;
  ep->etype = ELT_G;
  const char *b = findAttr(atts, "g:delim");
  if (plus_pending || '+' == *b)
    {
      char buf[strlen(utf8)+strlen(e_zwj.data)+1];
      strcpy(buf, utf8);
      strcat(buf, e_zwj.data);
      ep->data = hpool_copy((uccp)buf, c->hp);
      zwj_murub = 1;
      plus_pending = 0;
    }
  else
    ep->data = hpool_copy((uccp)utf8, c->hp);
  ep->btype = brk;
  ep->gtype = g;
  ep->c = cp;
  ep->oid = oid;
  ep->xid = (ccp)pool_copy((uccp)get_xml_id(atts), c->p);

  const char *g_o, *g_c;
  g_o = findAttr(atts, "g:o");
  g_c = findAttr(atts, "g:c");
  if (g_o && *g_o)
    ep->g_o = (ccp)hpool_copy(xmlify((uccp)g_o), c->hp);
  if (g_c && *g_c)
    ep->g_c = (ccp)hpool_copy(xmlify((uccp)g_c), c->hp);

  const char *gkey = findAttr(atts, "key");
  if (gkey && *gkey)
    {
      ep->key = (ccp)hpool_copy((uccp)gkey, c->hp);
      char *dot = strrchr(gkey, '.');
      if (dot[1])
	ep->title = dot+1;
      else
	ep->title = findAttrNULL(atts, "g:sign");
      if (ep->title)
	ep->title = (ccp)hpool_copy((uccp)ep->title, c->hp);
    }

  list_add(c->line, ep);

  const char *zwnj = findAttr(atts, "g:zwnj");
  if (zwnj && *zwnj)
    list_add(c->line, &e_zwnj);
}

static void
cfy_line(Cfy *c, const char **atts)
{
  Elt *ep = memo_new(c->m_elt);
  ep->etype = ELT_L;
  ep->prev = last_ep;
  last_ep = ep;
  curr_line = memo_new(c->m_line);
  curr_line->xid = (ccp)pool_copy((uccp)get_xml_id(atts), c->p);
  curr_line->label = (ccp)pool_copy((uccp)findAttr(atts, "label"), c->p);
  
  if (!strcmp(findAttr(atts, "type"), "lgs"))
    {
      c->cline = list_create(LIST_SINGLE);
      c->line = c->cline;
    }
  else
    {
      c->mline = list_create(LIST_SINGLE);
      c->line = c->mline;
    }
  
  ep->data = curr_line;
  list_add(c->line, ep);
  in_l = 1;
}

static void
cfy_x(Cfy *c, const char **atts, Btype brk, Class *cp)
{
  if (!c->line)
    c->line = list_create(LIST_DOUBLE);

  Elt *ep = memo_new(c->m_elt);
  ep->prev = last_ep;
  last_ep = ep;
  const char *gt = gt=findAttr(atts,"g:type");
  ep->xid = (ccp)pool_copy((uccp)get_xml_id(atts), c->p);
  if (!strcmp(gt, "ellipsis"))
    ep->etype = ELT_E;
  else if (!strcmp(gt, "newline"))
    ep->etype = ELT_R;
  else
    ep->etype = ELT_X;
  ep->btype = brk;
  ep->c = cp;
  
  list_add(c->line, ep);
}

static void
cfy_ws(Cfy *c)
{
  if (!c->line)
    c->line = list_create(LIST_DOUBLE);

  if (zws_pending)
    {
      list_add(c->line, &e_zws);
      zws_pending = 0;
    }
  else
    {
      Elt *ep = memo_new(c->m_elt);
      ep->etype = ELT_W;
      ep->prev = last_ep;
      last_ep = ep;
      prev_last_w = curr_line->last_w;
      curr_line->last_w = list_len(c->line);
      list_add(c->line, ep);
      /*fprintf(stderr, "cfy_ws last_w = %d\n", curr_line->last_w);*/
    }
}

void
cfy_reader_init(void)
{
  innertags['c'] = innertags['n'] = innertags['q'] = 1;
}

void
cfy_sH(void *userData, const char *name, const char **atts)
{
  Cfy *c = userData;
  
  if (!strcmp(name, "xcl"))
    longjmp(done, 0);

  if (!strcmp(name, "transliteration") || !strcmp(name, "composite"))
    {
      c->n = (ccp)xmlify((uccp)findAttr(atts, "n"));
      if (!c->project)
	{
	  const char *prj = findAttr(atts, "project");
	  if (*prj)
	    {
	      if (c->project && strcmp(prj, c->project))
		c->proxy = (ccp)pool_copy((uchar*)prj, c->p);
	      else
		c->project = (ccp)pool_copy((uchar*)prj, c->p);
	    }
	}
      Class *cp = cfy_class(((Cfy*)userData), findAttr(atts, "cfy-key"), curr_cp);
      if (cp)
	*curr_cp = *cp;
    }
  else if (!strcmp(name, "protocol") && !strcmp(findAttr(atts, "type"), "cfy"))
    {
      protocol_cfy_ccf = 1;
    }
  else
    {
      if (in_l && !inner)
	{
	  const char *utf8 = findAttr(atts, "g:utf8");
	  if (*utf8)
	    {
	      const char *oid = findAttr(atts, "spoid");
	      if (!oid || !*oid)
		oid = findAttr(atts, "key");
	      if (*oid)
		{
		  const char *dot = strchr(oid, '.');
		  if (dot)
		    {
		      char o[strlen(oid)+1];
		      strcpy(o, oid);
		      o[dot-oid] = '\0';
		      oid = (ccp)hpool_copy((ucp)o, c->hp);
		    }
		  else
		    oid = (ccp)hpool_copy((ucp)oid, c->hp);
		}
	      else
		oid = NULL;

	      Class *cp = cfy_class(((Cfy*)userData), findAttr(atts, "cfy-key"), curr_cp);
	      if (cp)
		*curr_cp = *cp;	      

	      cfy_grapheme(userData, name, atts, utf8, curr_cp, oid);
	    }
	  if (':' == name[1] && 'g' == name[0] && innertags[(int)name[2]])
	    ++inner;
	  else if (!strcmp(name, "g:w"))
	    {
	      const char *zws = findAttr(atts, "g:zws");
	      if (zws && *zws)
		zws_pending = 1;
	    }
	  else if (!strcmp(name, "c"))
	    cfy_cell(userData, atts);
	  else if (!strcmp(name, "g:d"))
	    {
	      const char *dd = findAttr(atts, "g:delim");
	      if ('+' == *dd)
		plus_pending = 1;
	    }
	  else if (!strcmp(name, "g:x"))
	    {
	      Class *cp = cfy_class(((Cfy*)userData), findAttr(atts, "cfy-key"), curr_cp);
	      if (cp)
		*curr_cp = *cp;	      
	      cfy_x(userData, atts, breakage(name, atts), curr_cp);
	    }
	}
      else if (!strcmp(name, "l"))
	{
	  cfy_line(userData, atts);
	}
    }
}

void
cfy_eH(void *userData, const char *name)
{
  if (!strcmp(name, "l"))
    {
      in_l = inner = 0;
      if (((Cfy*)userData)->cline)
	{
	  List *mline = list_pop(((Cfy*)userData)->body);
	  Line *mlineline = list_first(mline);
	  ((Cfy*)userData)->cline->first->data = mlineline;
	  list_add(((Cfy*)userData)->body, ((Cfy*)userData)->cline);
	}
      else if (((Cfy*)userData)->mline)
	list_add(((Cfy*)userData)->body, ((Cfy*)userData)->mline);

      /* if final Elt is wordspace pop it off */
      List *line = list_last(((Cfy*)userData)->body);
      Elt *ep = list_last(line);
      if (ep->etype == ELT_W)
	{
	  (void)list_pop(line);
	  curr_line->last_w = prev_last_w;
	  /*fprintf(stderr, "cfy_eH resetting last_w to %d\n", curr_line->last_w);*/
	}
      
      ((Cfy*)userData)->cline = ((Cfy*)userData)->mline = NULL;
    }
  else if (!strcmp(name, "protocol") && protocol_cfy_ccf)
    {
      Cfy *c = userData;
      if (c->proxy)
	c->proxypro_ccf = (ccp)pool_copy((uccp)charData_retrieve(), c->p);
      else
	c->protocol_ccf = (ccp)pool_copy((uccp)charData_retrieve(), c->p);
      protocol_cfy_ccf = 0;
      cfy_cfg_text(c);
    }
  else if (':' == name[1] && 'g' == name[0] && innertags[(int)name[2]])
    --inner;
  else if (!strcmp(name, "g:w"))
    cfy_ws(userData);
  else if (!strcmp(name, "c"))
    {
      Elt *ep = list_last(((Cfy*)userData)->line);
      if (ep->etype == ELT_W)
	{
	  (void)list_pop(((Cfy*)userData)->line);
	  curr_line->last_w = prev_last_w;
	  /*fprintf(stderr, "cfy_eH resetting last_w to %d\n", curr_line->last_w);*/
	}
    }
  else if (!strcmp(name, "g:f"))
    {
      if (ELT_G == last_ep->etype)
	{
	  const char *o = last_ep->otf = (ccp)pool_copy((uccp)charData_retrieve(), ((Cfy*)userData)->p);
	  if ((isdigit(o[0]) && (!o[1] || (isdigit(o[1]) && !o[2])))
	      || ('c' == o[0] && 'v' == o[1] && isdigit(o[2]) && isdigit(o[3]) && !o[4]))
	    ;
	  else
	    {
	      fprintf(stderr, "%s:%d: ignoring unknown or malformed OpenType feature %s\n", pi_file, pi_line, o);
	      last_ep->otf = NULL;
	    }
	}
	
    }
  (void)charData_discard();
}
