#include <oraccsys.h>
#include "isp.h"

void
isp_p3(Isp *ip, FILE *outfp)
{
  char const *fnlist[2];
  char fn[strlen(ip->oracc)+strlen("/lib/data/p3-template.xml0")];
  fnlist[0] = fn;
  fnlist[1] = NULL;
  runexpatNSuD(i_list, fnlist, ei_sH, ei_eH, NULL, outfp);
}

void
ida_p3content(Isp *ip)
{
  fprintf(stderr, "ida_p3content\n");
}

/* This printText implements the same escaping as used by oracc2's
   xmlify library routine */
void
printText(const char *s, FILE *frag_fp)
{
  if (s[1] || '@' != s[0]) /* Don't print the @ of <p>@</p> */
    {
      while (*s)
	{
	  if (*s == '<')
	    fputs("&lt;",frag_fp);
	  else if (*s == '>')
	    fputs("&gt;",frag_fp);
	  else if (*s == '&')
	    fputs("&amp;",frag_fp);
#if 0
	  else if (*s == '\'')
	    fputs("&apos;",frag_fp);
#endif
	  else if (*s == '"')
	    fputs("&quot;",frag_fp);
	  else if (*s == '@' && s[1] == '@')
	    s = ida_atat(s, frag_fp);
	  else
	    fputc(*s,frag_fp);
	  ++s;
	}
    }
}

void
printStart(FILE *fp, const char *name, const char **atts)
{
  const char **ap = atts;
  printText((const char*)charData_retrieve(), fp);
  fprintf(fp, "<%s", name);
  if (atts)
    {
      const char *id = NULL;
      for (ap = atts; ap[0]; )
	{
	  if (!strcmp(ap, "id"))
	    id=ap;
	  fprintf(fp, " %s=\"",*ap++);
	  printText(*ap++, fp);
	  fputc('"', fp);
	}
      if (id)
	{
	  struct idaction *iap = idactions(id, strlen(id));
	  if (iap)
	    isp->func(isp);
	  id = NULL;
	}
    }  
  fputc('>', fp);
}

void
printEnd(FILE *fp, const char *name)
{
  printText((const char *)charData_retrieve(), fp);
  fprintf(fp, "</%s>", name);
}

void
ei_sH(void *userData, const char *name, const char **atts)
{
  printStart(userData, name, atts);
}

void
ei_eH(void *userData, const char *name)
{
  printEnd(userData, name);
}

void
ida_at_identity(struct idactions *idap, FILE *fp)
{
  fputs(idap->name, fp);
}

void
ida_atatcgivar_adhoc(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_arg_item(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_asrch(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_fullscreen_0(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_glet(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_glos(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_glos(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_glosDisplay(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_gxis(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_item(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_list(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_page_1(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_page(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_pagesize(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_translation_en(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_transonly_0(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_transonly(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_uimode(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_unicode_0(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_unicode(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_uzpage(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_xipage(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatcgivar_zoom(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatga(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatoutlines(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatproject(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatprojectname(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatreferer(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_cetype_line(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_items(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_itemtype_off(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_mode(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_otlmode(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_outl_default(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_pages(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_prod(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_resultcount(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_sorttype(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_srchorig(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_srchtype_txt(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_tmpdir(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_type(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_what(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_znext(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atatruntime_zprev(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
void
ida_atattranslations(Isp *ip, struct idactions *idap, FILE *fp)
{
  ida_at_identity(idap, fp);
}
