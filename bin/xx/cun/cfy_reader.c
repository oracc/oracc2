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

static Elt *last_ep = NULL;

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
  if (c->coverage && !hash_find(c->c->fntp->uni_seen, (uccp)utf8))
    {
      hash_add(c->c->fntp->uni_seen, (uccp)utf8, "");
      if (!cfy_uni_check(c, (uccp)utf8))
	fprintf(stderr,
		"%s:%d: font %s missing %s=%s=%s\n",
		pi_file, pi_line, c->c->fntp->full, utf8, oid, findAttr(atts, "g:sign"));
    }
  
  Btype brk = breakage(name, atts);
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

  /* add ZWJ/ZWNJ if the boundary requires it */
  const char *b = findAttr(atts, "g:boundary");
  if ('+' == *b)
    list_add(c->line, &e_zwj);
  else if ('\\' == *b)
    list_add(c->line, &e_zwnj);
}

static void
cfy_line(Cfy *c, const char **atts)
{
  Elt *ep = memo_new(c->m_elt);
  ep->etype = ELT_L;
  ep->prev = last_ep;
  last_ep = ep;
  Line *lp = memo_new(c->m_line);
  lp->xid = (ccp)pool_copy((uccp)get_xml_id(atts), c->p);
  lp->label = (ccp)pool_copy((uccp)findAttr(atts, "label"), c->p);
  /* TODO: test for colon-line here and add first list node of userData-line to it */
  c->line = list_create(LIST_SINGLE);
  ep->data = lp;
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
  ep->etype = (!strcmp(gt,"ellipsis") ? ELT_E : ELT_X);
  ep->btype = brk;
  ep->c = cp;
  
  list_add(c->line, ep);

  /* add ZWJ/ZWNJ if the boundary requires it */
  const char *b = findAttr(atts, "g:boundary");
  if ('+' == *b)
    list_add(c->line, &e_zwj);
  else if ('\\' == *b)
    list_add(c->line, &e_zwnj);
}

static void
cfy_ws(Cfy *c)
{
  if (!c->line)
    c->line = list_create(LIST_DOUBLE);

  Elt *ep = memo_new(c->m_elt);
  ep->etype = ELT_W;
  ep->prev = last_ep;
  last_ep = ep;
  
  list_add(c->line, ep);
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
	  else if (!strcmp(name, "c"))
	    cfy_cell(userData, atts);
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
	list_add(((Cfy*)userData)->body, ((Cfy*)userData)->cline);
      else if (((Cfy*)userData)->line)
	list_add(((Cfy*)userData)->body, ((Cfy*)userData)->line);

      /* if final Elt is wordspace pop it off */
      List *line = list_last(((Cfy*)userData)->body);
      Elt *ep = list_last(line);
      if (ep->etype == ELT_W)
	(void)list_pop(line);
      
      ((Cfy*)userData)->cline = ((Cfy*)userData)->line = NULL;
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
	(void)list_pop(((Cfy*)userData)->line);
    }
  (void)charData_discard();
}
