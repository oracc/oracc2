#include <oraccsys.h>
#include "ofp.h"

static void xml_list(Ofp *ofp, List *lp, const char *gtag, const char *itag, FILE *fp);
static void xml_sign(Ofp *ofp, const char *sn, FILE *fp);

void
ofp_xml(Ofp *ofp, FILE *fp)
{
  fprintf(fp, "<ofp n=\"%s\" f=\"%s\">", ofp->name, ofp->file);
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
      fprintf(fp, "<liga n=\"%s\">", gp->glyph->liga);
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
xml_sign(Ofp *ofp, const char *sn, FILE *fp)
{
  fprintf(fp, "<sign xml:id=\"x%s\" utf8=\"%s\">", sn, utf8ify(strtoul(sn,NULL,16)));
  Ofp_sign *osp = hash_find(ofp->h_sign, (uccp)sn);
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
