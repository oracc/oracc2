#include <oraccsys.h>
#include <xmlify.h>
#include "ofp.h"

static void xml_list(Ofp *ofp, List *lp, const char *gtag, const char *itag, FILE *fp);
static void xml_sign(Ofp *ofp, const char *sn, FILE *fp);
static const char *xutf8_liga_literal(Ofp *o, Ofp_glyph *gp);
static const char *xutf8_of(Ofp *o, Ofp_glyph *gp);
static void xml_header(Ofp_header *hp, FILE *fp);

const char *ofp_ns = "http://oracc.org/ns/ofp/1.0";

void
ofp_xml(Ofp *ofp, FILE *fp)
{
  fprintf(fp, "<ofp xmlns=\"%s\" xmlns:ofp=\"%s\" n=\"%s\" f=\"%s\"",
	  ofp_ns, ofp_ns, ofp->name, ofp->file);
  xml_header(&ofp->h, fp);
  fputc('>', fp);
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
xml_attr(const char *n, const char *v, FILE *fp)
{
  if (v)
    fprintf(fp, " %s=\"%s\"", n, xmlify((uccp)v));
}

static void
xml_header(Ofp_header *hp, FILE *fp)
{
  xml_attr("title", hp->title, fp);
  xml_attr("label", hp->label, fp);
  xml_attr("page", hp->page, fp);
  xml_attr("font", hp->font, fp);
  xml_attr("css", hp->css, fp);
  xml_attr("mag", hp->mag, fp);
  xml_attr("list", hp->list, fp);
  xml_attr("data", hp->data, fp);
}

static void
xml_liga(Ofp *ofp, List *lp, FILE *fp)
{
  fputs("<ligas>", fp);
  Ofp_liga *gp;
  for (gp = list_first(lp); gp; gp = list_next(lp))
    {
      fprintf(fp, "<liga n=\"%s\" utf8=\"%s\" l=\"%s\" zwnj=\"%s\">",
	      xmlify(osl_seq_name(ofp->osl, gp->glyph->liga, ofp->p)),
	      xutf8_liga_literal(ofp, gp->glyph),
	      gp->glyph->liga,
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

static void
xml_list2(Ofp *ofp, List *lp, const char *gtag, const char *itag, FILE *fp)
{
  fprintf(fp, "<%s>", gtag);
  const char *f;
  for (f = list_first(lp); f; f = list_next(lp))
    fprintf(fp, "<%s>%s</%s>", itag, f, itag);
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
list_features(char *f, List **salt, List **sset, List **cvnn)
{
  List *sa = list_create(LIST_SINGLE), *ss = list_create(LIST_SINGLE), *cv = list_create(LIST_SINGLE);
  while (*f)
    {
      while (isspace(*f))
	*f++ = '\0';
      if ('.' == *f)
	list_add(sa, ++f);
      else if ('s' == *f)
	list_add(ss, f);
      else if ('c' == *f)
	list_add(cv, f);
      while (*f && !isspace(*f))
	++f;
    }
  if (list_len(sa))
    *salt = sa;
  else
    *salt = NULL;
  if (list_len(ss))
    *sset = ss;
  else
    *sset = NULL;
  if (list_len(cv))
    *cvnn = cv;
  else
    *cvnn = NULL;
}

static void
xml_sign(Ofp *ofp, const char *sn, FILE *fp)
{
  Ofp_sign *osp = hash_find(ofp->h_sign, (uccp)sn);

  if (!osp->glyph->osl)
    return;
  
  const char *xid = xid_of(ofp, osp->glyph);

#if 1
  const char *liga_utf8 = NULL;
  if (osp->glyph->liga)
    liga_utf8 = xutf8_liga_literal(ofp, osp->glyph);
#else
  const char *liga_n = NULL;
  const char *liga_zwnj = NULL;
  if (osp->ligas)
    {
      Ofp_liga *gp = list_first(osp->ligas);
      liga_n = gp->glyph->liga;
      liga_utf8 = xutf8_liga_literal(ofp, gp->glyph);
      liga_zwnj = xutf8_of(ofp, gp->glyph);
      osp->ssets = gp->ssets;
    }
#endif
  
  fprintf(fp, "<sign xml:id=\"%s\" utf8=\"%s\"", xid,
	  liga_utf8 ? liga_utf8 : xutf8_of(ofp, osp->glyph));
  
  if (osp->glyph->liga)
    fprintf(fp, " l=\"%s\" zwnj=\"%s\"", osp->glyph->liga, xutf8_of(ofp, osp->glyph));

  if (osp->glyph->osl)
    {
      Ofp_list *olp = NULL;
      fprintf(fp, " oid=\"%s\" n=\"%s\" sort=\"%d\"",
	      osp->glyph->osl->o, xmlify(osp->glyph->osl->n), osp->glyph->osl->s);
      fputc('>', fp);
      if (ofp->list && (olp = hash_find(ofp->h_list, (uccp)osp->glyph->osl->o)))
	{
	  fputs("<lists>", fp);
	  while (olp)
	    {
	      fprintf(fp, "<list l=\"%s\" s=\"%d\"", olp->l, olp->s);
	      if (olp->f)
		{
		  fprintf(fp, " f=\"%s\">", olp->f);
		  List *fsalt, *fsset, *fcvnn;
		  char *f = strdup(olp->f);
		  list_features(f, &fsalt, &fsset, &fcvnn);
		  if (fsset)
		    {
		      xml_list2(ofp, fsset, "ssets", "sset", fp);
		      list_free(fsset, NULL);
		    }
		  if (fcvnn)
		    {
		      xml_list2(ofp, fcvnn, "cvnns", "cvnn", fp);
		      list_free(fcvnn, NULL);
		    }
		  if (fsalt)
		    {
		      xml_list2(ofp, fsalt, "salts", "salt", fp);
		      list_free(fsalt, NULL);
		    }		      
		  free(f);
		}
	      else
		fputc('>', fp);
	      fputs("</list>", fp);
	      olp = olp->next;
	    }
	  fputs("</lists>", fp);	  
	}
    }
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
