#include <oraccsys.h>
#include <roco.h>
#include <xml.h>
#include <oraccns.h>
#include "cx.h"

/* Hooks for Roco library to generate backward-compatible XMD */

static void
cx_roco_outer_dest(Roco *r, int i, FILE *fp)
{
  char *dest = expand(((struct cx_xml_user *)r->user)->c->project, (ccp)r->rows[i][0], "xmd");
  fprintf(fp, "<?destfile %s?>\n", dest);
}

static void
cx_roco_outer_none(Roco *r, int i, FILE *fp)
{
  fputs("<?destfile #none?>\n", fp);
}

static const char *
cx_langmask(Cx *c, Roco *r, int i)
{
  int langfld = cx_roco_field_index(r, "language");
  if (-1 == langfld)
    langfld = cx_roco_field_index(r, "lang");
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
#define cxp(ru) (((struct cx_xml_user*)(ru->user))->c)
#define fcp(ru) (((struct cx_xml_user*)(ru->user))->f)
  fputs("<cat>", fp);
  for (j = 0; j < r->maxcols; ++j)
    {
      if (ftags)
	ctag = ftags[j];
      if (fcp(r)[i][j].type == FCELL_SORT)
	{
	  fprintf(fp, "<%s c=\"%ld\">%s</%s>",
		  ctag, fcp(r)[i][j].sort,
		  xmlify(ipool_str(cxp(r)->si_pool,fcp(r)[i][j].u.index)),
		  ctag);
	}
      else
	{
	  const char *s = fcp(r)[i][j].u.str;
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
  roco_xml_row_hooks(cx_roco_outer_dest, cx_roco_row);
}

void
cx_roco_outer(Cx *c)
{
  roco_xml_row_hooks(cx_roco_outer_none, cx_roco_row);
}

int
cx_roco_field_index(Roco *r, const char *fld)
{
  int f;
  for (f = 0; r->rows[0][f]; ++f)
    {
      if (!strcmp((ccp)r->rows[0][f], fld))
	return f;
    }
  return -1;
}

int
cx_roco_id_index(Roco *r)
{
  int f;
  for (f = 0; r->rows[0][f]; ++f)
    {
      uccp f1 = r->rows[0][f];
      if (!strcmp((ccp)f1, "id_text")
	  || !strcmp((ccp)f1, "id_composite")
	  || !strcmp((ccp)f1, "o:id"))
	return f;
    }
  return -1;
}
