#include <oraccsys.h>
#include "cfy.h"

char innertags[128];

Class cfy_defaults = { .fnt="noto" , .set=NULL, .mag="100", .scr="middle", .asl="osl" };
Class *curr_cp = &cfy_defaults;

List *cfy_line; /* A list of pointers to Elts; initially the XTF line, but
	       reset and rebuilt from the sequence line if there is
	       one. */

#define elt_grapheme(listnode) (((Elt*)listnode->data)->etype == ELT_G)

Memo *m_elt;
Memo *m_class;
Hash *lig;
Pool *hp;

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
  if (!line)
    line = list_create(LIST_DOUBLE);

  Elt *ep = memo_new(c->m_elt);
  ep->etype = ELT_G;
  ep->u8 = utf8;
  ep->btype = brk;
  ep->c = cp;
  ep->oid = oid;
  
  list_add(line, ep);

  /* add ZWJ/ZWNJ if the boundary requires it */
  const char *b = findAttr(atts, "g:boundary");
  if ('+' == *b)
    list_add(line, e_zwj);
  else if ('\\' == *b)
    list_add(line, e_zwnj);
}

void
cfy_reader_init(void)
{
  innertags['c'] = innertags['n'] = innertags[q] = 1;
  m_elt = memo_init(sizeof(Elt), 1024);
  m_class = memo_init(sizeof(Class), 8);
  hp = hpool_create();
}

void
cfy_reader_reset(void)
{
  memo_reset(m_elt);
}

void
cfy_reader_term(void)
{
  memo_free(m_elt);
  memo_free(m_class);
  if (lig)
    hash_free(lig);
  pool_term(p);
  pool_term(hp);
}

void
cfy_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "xcl"))
    longjmp(done, 0);

  if (!strcmp(name, "transliteration") || !strcmp(name, "composite"))
    {
      const char *xn = (ccp)xmlify((uccp)findAttr(atts, "n"));
      if (!project)
	{
	  const char *prj = findAttr(atts, "project");
	  if (*prj)
	    project = (ccp)pool_copy((uchar*)prj, p);
	}
      cfy_class *cp = cfy_class(findAttr(atts, "cfy"), curr_cp);
      if (cp)
	*curr_cp = *cp;
	
      cfy_head(outfp, xn, cp);
    }
  else
    {
      if (in_l && !inner)
	{
	  const char *utf8 = findAttr(atts, "utf8");
	  if (*utf8)
	    {
	      cfy_class *cp = cfy_class(findAttr(atts, "cfy"), curr_cp);
	      const char *oid = findAttr(atts, "spoid");
	      if (!oid || !*oid)
		oid = findAttr(atts, "key");
	      if (*oid)
		{
		  const char *dot = strchr(oid, '.');
		  if (dot)
		    {
		      char o[strlen(oid)+1];
		      o[dot-oid] = '\0';
		      oid = (ccp)hpool_copy((ucp)o, hp);
		    }
		  else
		    oid = (ccp)hpool_copy((ucp)oid, hp);
		}
	      else
		oid = NULL;

	      cfy_class *cp = cfy_class(findAttr(atts, "cfy"), curr_cp);
	      if (cp)
		*curr_cp = *cp;	      

	      cfy_grapheme(atts, utf8, breakage(atts), curr_cp, oid);
	    }
	  else if (':' == name[1] && 'g' == name[0] && innertags[name[2]])
	    ++inner;
	  else if (!strcmp(name, "g:x"))
	    cfy_x(atts, breakage(atts), cfy_class(atts, curr_cp));

	}
    }
  if (!strcmp(name, "l"))
    {
      if (cfy_line)
	cfy_line_term();
      cfy_line_init();
    }
}

void
cfy_eH(void *userData, const char *name)
{
  if (!strcmp(name, "l"))
    in_l = inner = 0;
  else if (':' == name[1] && 'g' == name[0] && innertags[name[2]])
    --inner;
  else if (!strcmp(name, "g:q"))
    in_q = 0;
  else if (!strcmp(name, "g:c"))
    in_c = 0;
  else if (!strcmp(name, "g:w"))
    cfy_word_space();
}
