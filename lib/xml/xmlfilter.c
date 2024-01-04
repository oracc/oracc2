#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hash.h>
#include <pool.h>
#include "runexpat.h"

static Hash *actqueue = NULL;
static Pool *p = NULL;

struct xf_actions
{
  xf_action_attr attr;
  xf_action_func firstChild;
  xf_action_func lastChild;
} acts = { NULL, NULL, NULL };

/* This printText implements the same escaping as used by oracc2's
   xmlify library routine */
static void
xf_printText(const char *s, FILE *fp)
{
  while (*s)
    {
      if (*s == '<')
	fputs("&lt;",fp);
      else if (*s == '>')
	fputs("&gt;",fp);
      else if (*s == '&')
	fputs("&amp;",fp);
#if 0
      else if (*s == '\'')
	fputs("&apos;",fp);
#endif
      else if (*s == '"')
	fputs("&quot;",fp);
      else
	fputc(*s,fp);
      ++s;
    }
}

static void
xf_sH(void *fp, const char *name, const char **atts)
{
  xf_printText((const char*)charData_retrieve(), fp);

  const char *xid = get_xml_id(atts);

  fprintf(fp, "<%s", name);
  if (atts)
    {
      const char **ap;
      if (*xid && acts.attr)
	ap = acts.attr(xid, name, atts, fp);
      else
	ap = atts;
      while (ap[0])
	{
	  fprintf(fp, " %s=\"",*ap++);
	  xf_printText(*ap++, fp);
	  fputc('"', fp);
	}
    }
  fputc('>', fp);
  if (*xid)
    {
      if (acts.firstChild)
	acts.firstChild(xid, name, atts, fp);
      if (acts.lastChild)
	{
	  if (!hash_find(actqueue, (ucp)name))
	    name = (ccp)pool_copy((ucp)name, p);
	  hash_add(actqueue, (ucp)name, pool_copy((ucp)xid, p));
	}
    }
}

static void
xf_eH(void *fp, const char *name)
{
  xf_printText((const char *)charData_retrieve(), fp);
  const char *xid = hash_find(actqueue, (ucp)name);
  if (xid)
    {
      if (acts.lastChild)
	{
	  acts.lastChild(xid, name, NULL, fp);
	  hash_add(actqueue, (ucp)name, NULL);
	}
    }
  fprintf(fp, "</%s>", name);
}

void
xmlfilter_actions(xf_action_attr attr, xf_action_func firstChild, xf_action_func lastChild)
{
  acts.attr = attr;
  acts.firstChild = firstChild;
  acts.lastChild = lastChild;
}

void
xmlfilter(const char *fname, FILE *outfp)
{
  char const *fnlist[2];
  enum isource isrc = 0;

  if (fname)
    {
      fnlist[0] = fname;
      fnlist[1] = NULL;
      isrc = i_list;
    }
  else
    isrc = i_stdin;
  
  actqueue = hash_create(32);
  p = pool_init();
  runexpat_omit_rp_wrap();
  runexpatNSuD(isrc, fnlist, xf_sH, xf_eH, NULL, outfp);
  hash_free(actqueue, NULL);
  pool_term(p);
}
