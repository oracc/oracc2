#include <oraccsys.h>
#include <xml.h>
#include <runexpat.h>
#include "isp.h"

struct ei_user_data
{
  Isp *ip;
  FILE *fp;
};

void
ei_commentH(void*userData, const char *cmt)
{
  const char *pre = charData_retrieve();
  if (pre)
    fputs(pre, ((struct ei_user_data*)userData)->fp);
  fprintf(((struct ei_user_data*)userData)->fp, "<!--%s-->", cmt);
}

int
isp_p3(Isp *ip, FILE *outfp)
{
  char const *fnlist[2];
  char fn[strlen(ip->oracc)+strlen("/lib/data/p3-template.xml0")];
  sprintf(fn, "%s/lib/data/p3-template.xml", ip->oracc);
  fnlist[0] = fn;
  fnlist[1] = NULL;
  runexpatCommentHandler = ei_commentH;
  struct ei_user_data ud;
  ud.ip = ip;
  ud.fp = outfp;
  runexpat_omit_rp_wrap();
  runexpatNSuD(i_list, fnlist, ei_sH, ei_eH, NULL, &ud);
  return 0;
}

void
ida_p3content(Isp *ip, struct idactions *idap, FILE *fp)
{
  fprintf(fp, "ida_p3content\n");
}

/* This function is called when iterating over text one character at a
   time; it needs to return a pointer to the last character output
   because the return pointer is then incremented */
static const char *
ida_atat(Isp *ip, FILE *fp,  const char *at)
{
  const char *atend = strstr(at+2, "@@");
  if (atend)
    {
      atend += 2;
      char buf[(atend-at)+1];
      strncpy(buf, at, atend-at);
      buf[atend-at] = '\0';
      struct idactions *idap = idactions(buf, atend - at);      
      if (idap)
	idap->func(ip, idap, fp);
      else
	while (at < atend)
	  fputs(buf, fp);
      return atend - 1;
    }
  else
    {
      fputc('@', fp);
      return at;
    }
}

/* This printText implements the same escaping as used by oracc2's
   xmlify library routine */
void
printText(Isp *ip, FILE *fp, const char *s)
{
  if (s[1] || '@' != s[0]) /* Don't print the @ of <p>@</p> */
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
	  else if (*s == '@' && s[1] == '@')
	    s = ida_atat(ip, fp, s);
	  else
	    fputc(*s,fp);
	  ++s;
	}
    }
}

void
printStart(Isp *ip, FILE *fp, const char *name, const char **atts)
{
  const char **ap = atts;
  printText(ip, fp, (ccp)charData_retrieve());
  fprintf(fp, "<%s", name);
  const char *id = NULL;
  if (atts)
    {
      for (ap = atts; ap[0]; )
	{
	  if (!strcmp(ap[0], "id"))
	    id=ap[1];
	  fprintf(fp, " %s=\"",*ap++);
	  printText(ip, fp, *ap++);
	  fputc('"', fp);
	}
    }  
  fputc('>', fp);
  if (id)
    {
      struct idactions *idap = idactions(id, strlen(id));
      if (idap)
	idap->func(ip, idap, fp);
      id = NULL;
    }
}

void
printEnd(Isp *ip, FILE *fp, const char *name)
{
  printText(ip, fp, (const char *)charData_retrieve());
  fprintf(fp, "</%s>", name);
}

void
ei_sH(void *userData, const char *name, const char **atts)
{
  printStart(((struct ei_user_data *)userData)->ip, ((struct ei_user_data *)userData)->fp, name, atts);
}

void
ei_eH(void *userData, const char *name)
{
  printEnd(((struct ei_user_data *)userData)->ip, ((struct ei_user_data *)userData)->fp, name);
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
ida_atatcgivar_glos_bis(Isp *ip, struct idactions *idap, FILE *fp)
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
