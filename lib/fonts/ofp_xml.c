#include <oraccsys.h>
#include <xmlify.h>
#include "ofp.h"

static void xml_list(Ofp *ofp, List *lp, const char *gtag, const char *itag, FILE *fp);
static void xml_sign(Ofp *ofp, const char *sn, FILE *fp);
static const char *xutf8_liga_literal(Ofp *o, Ofp_glyph *gp);
static const char *xutf8_of(Ofp *o, Ofp_glyph *gp);

const char *ofp_ns = "http://oracc.org/ns/ofp/1.0";

void
ofp_xml(Ofp *ofp, FILE *fp)
{
  fprintf(fp, "<ofp xmlns=\"%s\" xmlns:ofp=\"%s\" n=\"%s\" f=\"%s\">",
	  ofp_ns, ofp_ns, ofp->name, ofp->file);
  int nsign;
  const char **k = hash_keys2(ofp->h_sign, &nsign);
  qsort(k, nsign, sizeof(char*), cmpstringp);
  int i;
  for (i = 0; k[i]; ++i)
    {
      if ('-' != k[i][0])
	xml_sign(ofp, k[i], fp);
    }
  fprintf(fp, "</ofp>");
}

static void
xml_liga(Ofp *ofp, List *lp, FILE *fp)
{
  fputs("<ligas>", fp);
  Ofp_liga *gp;
  for (gp = list_first(lp); gp; gp = list_next(lp))
    {
      fprintf(fp, "<liga n=\"%s\" utf8=\"%s\" zwnj=\"%s\">",
	      gp->glyph->liga,
	      xutf8_liga_literal(ofp, gp->glyph),
	      xutf8_of(ofp, gp->glyph));
      if (gp->ssets)
	xml_list(ofp, gp->ssets, "ssets", "sset", fp);
      if (gp->cvnns)
	xml_list(ofp, gp->cvnns, "cvnns", "cvnn", fp);
      if (gp->salts)
	xml_list(ofp, gp->salts, "salts", "salt", fp);
      fprintf(fp, "</liga>");
    }
  fputs("</ligas>", fp);
}

static void
xml_list(Ofp *ofp, List *lp, const char *gtag, const char *itag, FILE *fp)
{
  fprintf(fp, "<%s>", gtag);
  Ofp_glyph *gp;
  if (!strcmp(itag, "salt"))
    for (gp = list_first(lp); gp; gp = list_next(lp))
      fprintf(fp, "<%s>%d</%s>", itag, gp->f_int, itag);
  else
    for (gp = list_first(lp); gp; gp = list_next(lp))
      fprintf(fp, "<%s>%s</%s>", itag, gp->f_chr, itag);
  fprintf(fp, "</%s>", gtag);
}

static const char *
xutf8_useq(Ofp *o, const char *useq, int zwnj)
{
  static const char *u200c = "\xE2\x80\x8C";
  int n = 1;
  const char *lp = useq;
  while (*++lp)
    if ('.' == *lp)
      ++n;
  char buf[n*20]; /* utf8 char + u200C between each pair plus some slack */
  lp = useq;
  *buf = '\0';
  while (lp)
    {
      if (zwnj && !strncmp(lp,"xE01",4))
	strcat(strcat(buf, "_U+"), ++lp);
      else
	strcat(buf, (char*)utf8ify(strtoul(++lp,NULL,16)));
      if (zwnj && !strstr(buf, "_U+"))
	strcat(buf, u200c);
      lp = strchr(lp, '.');
      if (lp)
	++lp;
    }
  return (ccp)pool_copy((uccp)buf, o->p);
}

static const char *
xutf8_of(Ofp *o, Ofp_glyph *gp)
{
  if (gp->liga)
    {
      if (!gp->useq)
	gp->useq = liga2useq(o, gp->liga);
      return xutf8_useq(o, gp->useq, 1);
    }
  else
    {
      return (ccp)pool_copy(utf8ify(strtoul(gp->key,NULL,16)), o->p);
    }
}

static const char *
xutf8_liga_literal(Ofp *o, Ofp_glyph *gp)
{
  if (!gp->useq)
    gp->useq = liga2useq(o, gp->liga);
  return xutf8_useq(o, gp->useq, 0);
}

static const char *
xid_of(Ofp *o, Ofp_glyph *gp)
{
  if (gp->liga)
    {
      if (!gp->useq)
	gp->useq = liga2useq(o, gp->liga);
      return gp->useq;
    }
  else
    {
      char buf[strlen(gp->key)+2];
      sprintf(buf, "x%s", gp->key);
      return (ccp)pool_copy((uccp)buf, o->p);
    }
}

static void
xml_sign(Ofp *ofp, const char *sn, FILE *fp)
{
  Ofp_sign *osp = hash_find(ofp->h_sign, (uccp)sn);
  const char *xid = xid_of(ofp, osp->glyph);
  fprintf(fp, "<sign xml:id=\"%s\" utf8=\"%s\"", xid, xutf8_of(ofp, osp->glyph));
  if (osp->glyph->osl)
    fprintf(fp, " oid=\"%s\" n=\"%s\">", osp->glyph->osl->o, xmlify(osp->glyph->osl->n));
  else
    fputc('>', fp);
  if (osp->ssets)
    xml_list(ofp, osp->ssets, "ssets", "sset", fp);
  if (osp->cvnns)
    xml_list(ofp, osp->cvnns, "cvnns", "cvnn", fp);
  if (osp->salts)
    xml_list(ofp, osp->salts, "salts", "salt", fp);
  if (osp->ligas)
    xml_liga(ofp, osp->ligas, fp);
  fputs("</sign>", fp);
}
