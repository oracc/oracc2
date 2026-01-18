#include <oraccsys.h>
#include <roco.h>
#include <xml.h>
#include <oraccns.h>
#include "cx.h"

/* Hooks for Roco library to generate backward-compatible XMD */

static void
cx_roco_outer(Roco *r, int i, FILE *fp)
{
  char *dest = expand(((Cx *)r->user)->project, (ccp)r->rows[i][0], "xmd");
  fprintf(fp, "<?destfile %s?>\n", dest);
}

static const char *
cx_langmask(Cx *c, Roco *r, int i)
{
  int langfld = cx_roco_field_index(c, "language");
  if (-1 == langfld)
    langfld = cx_roco_field_index(c, "language");
  if (langfld > 0)
    {
      char *ls = strdup((ccp)r->rows[i][langfld]), *s = ls;
      while (*s)
	*s = tolower(*s);
      char **ll = space_split(ls);
      size_t l = 0;
      int i;
      for (i = 0; ll[i]; ++i)
	{
	  if (!strcmp(ll[i], "akkadian"))
	    l |= LM_AKK;
	  else if (!strcmp(ll[i], "sumerian"))
	    l |= LM_AKK;
	  else if (!strcmp(ll[i], "proto-cuneiform"))
	    l |= LM_PCUN;
	  else if (!strcmp(ll[i], "proto-elamite"))
	    l |= LM_PCUN;
	  else
	    l |= LM_MISC;
	}
      char buf[strlen(" langs=\"\"0")+9];
      sprintf(buf, " langs=\"%08lx\"", l);
      return strdup(buf);
    }
  return NULL;
}

static void
cx_roco_row(Roco *r, int i, FILE *fp)
{
  int j;
  const char **ftags = (r->fields_from_row1
			? (const char **)r->rows[0] : NULL);
  const char *ctag = r->celtag;
  const char *lmattr = cx_langmask(r->user, r, i);
  fprintf(fp, "<xmd xmlns=\"%s\" xmlns:xmd=\"%s\"", NS_XMD, NS_XMD);
  if (lmattr)
    fputs(lmattr, fp);
  fputc('>', fp);
#define cxp(ru) ((Cx*)(ru->user))
  fputs("<cat>", fp);
  for (j = 0; j < r->maxcols; ++j)
    {
      if (ftags)
	ctag = ftags[j];
      if (cxp(r)->si_rows[i][j].type == FCELL_SORT)
	{
	  fprintf(fp, "<%s c=\"%ld\">%s</%s>",
		  ctag, cxp(r)->si_rows[i][j].sort,
		  xmlify(ipool_str(cxp(r)->si_pool,cxp(r)->si_rows[i][j].u.index)),
		  ctag);
	}
      else
	{
	  const char *s = cxp(r)->si_rows[i][j].u.str;
	  if (*s)
	    fprintf(fp, "<%s>%s</%s>", ctag, xmlify((uccp)s), ctag);
	}
    }
  fputs("</cat></xmd>", fp);
#undef cxp
}

void
cx_roco(Cx *c)
{
  roco_xmd_ns = 1;
  roco_xml_row_hooks(cx_roco_outer, cx_roco_row);
}

int
cx_roco_field_index(Cx *c, const char *fld)
{
  int f;
  for (f = 0; c->r->rows[0][f]; ++f)
    {
      if (!strcmp((ccp)c->r->rows[0][f], fld))
	return f;
    }
  return -1;
}

int
cx_roco_id_index(Cx *c)
{
  int f;
  for (f = 0; c->r->rows[0][f]; ++f)
    {
      uccp f1 = c->r->rows[0][f];
      if (!strcmp((ccp)f1, "id_text")
	  || !strcmp((ccp)f1, "id_composite")
	  || !strcmp((ccp)f1, "o:id"))
	return f;
    }
  return -1;
}
