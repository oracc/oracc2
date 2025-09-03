#include <oraccsys.h>
#include <setjmp.h>
#include <runexpat.h>
#include <xml.h>
#include "cfy.h"

int in_c = 0, in_l = 0, in_n = 0, in_q = 0, inner = 0;

char innertags[128];

Elt e_zwj = { .etype=ELT_J, .cun=(uccp)"\xE2\x80\x8D" };
Elt e_zwnj = { .etype=ELT_N, .cun=(uccp)"\xE2\x80\x8C" };

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
cfy_grapheme(Cfy *c, const char **atts, const char *utf8, Btype brk, Class *cp, const char *oid)
{
  if (!c->line)
    c->line = list_create(LIST_DOUBLE);

  Elt *ep = memo_new(c->m_elt);
  ep->etype = ELT_G;
  ep->cun = hpool_copy((uccp)utf8, c->hp);
  ep->btype = brk;
  ep->c = cp;
  ep->oid = oid;
  ep->xid = (ccp)pool_copy((uccp)get_xml_id(atts), c->p);
  
  list_add(c->line, ep);

  /* add ZWJ/ZWNJ if the boundary requires it */
  const char *b = findAttr(atts, "g:boundary");
  if ('+' == *b)
    list_add(c->line, &e_zwj);
  else if ('\\' == *b)
    list_add(c->line, &e_zwnj);
}

static void
cfy_x(Cfy *c, const char **atts, Btype brk, Class *cp)
{
  if (!c->line)
    c->line = list_create(LIST_DOUBLE);

  Elt *ep = memo_new(c->m_elt);
  ep->etype = ELT_X;
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
	    c->project = (ccp)pool_copy((uchar*)prj, c->p);
	}
      Class *cp = cfy_class(((Cfy*)userData), findAttr(atts, "cfy-key"), curr_cp);
      if (cp)
	*curr_cp = *cp;
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

	      cfy_grapheme(userData, atts, utf8, breakage(name, atts), curr_cp, oid);
	    }
	  else if (':' == name[1] && 'g' == name[0] && innertags[(int)name[2]])
	    ++inner;
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
	  Line *lp = memo_new(c->m_line);
	  lp->xid = (ccp)pool_copy((uccp)get_xml_id(atts), c->p);
	  lp->label = (ccp)pool_copy((uccp)findAttr(atts, "label"), c->p);
	  /* TODO: test for colon-line here and add first list node of userData-line to it */
	  c->line = list_create(LIST_SINGLE);
	  list_add(c->line, lp);
	  in_l = 1;
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
  else if (':' == name[1] && 'g' == name[0] && innertags[(int)name[2]])
    --inner;
  else if (!strcmp(name, "g:q"))
    in_q = 0;
  else if (!strcmp(name, "g:c"))
    in_c = 0;
  else if (!strcmp(name, "g:w"))
    cfy_ws(userData);
}
