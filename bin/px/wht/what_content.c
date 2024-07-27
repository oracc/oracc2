#include <oraccsys.h>
#include <runexpat.h>
#include <glob.h>
#include "../isp/isp.h"
#include "../pui/pui.h"
#include "../pxdefs.h"
#include "what.h"

struct what_frag
{
  int nesting;
  const char *xid;
  FILE *fp;
  struct content_opts *cop;
  Isp *ip;
};

static int cued_printStart = 0, need_gdf_closer = 0;

extern int glob_pattern(const char *pat, glob_t *gres);
static void content_eH(void *userData, const char *name);
static void content_sH(void *userData, const char *name, const char **atts);
static void printEnd(struct what_frag *frag, const char *name);
static void printHTMLStart(struct what_frag *frag);
static void printStart(struct what_frag *frag, const char *name, const char **atts, const char *xid);
static void printText(const char *s, FILE *frag_fp);
static char *set_p4pager(Isp *ip);

static int cop_output(Isp *ip, struct content_opts *cop, const char *input);
static int pxr_output(Isp *ip);

#if 0
static const char *html_wild(const char *input);
#endif

int
what_content(Isp *ip, struct content_opts *cop, const char *input)
{
  if (input)
    cop_output(ip, cop, input);
  else
    pxr_output(ip);

  exit(0);
}

static int
pxr_output(Isp *ip)
{
  if (!strcmp(ip->part, "full"))
    return pui_output(ip, stdout, pui_filetext("p4full.xml"));
  else if (!strcmp(ip->part, "plus"))
    return pui_output(ip, stdout, pui_filetext("p4plus.xml"));
  else
    ; /* futured */
  return 0;
}

static int
cop_output(Isp *ip, struct content_opts *cop, const char *input)
{
  char const *fnlist[2];
  static struct what_frag fragdata;

  fragdata.ip = ip;
  fragdata.cop = cop;
  fragdata.xid = cop->chunk_id;
  fragdata.nesting = 0;
  fragdata.fp = stdout;
  fnlist[0] = input;
  fnlist[1] = NULL;

  /* Setting fragdata.nesting here means that the normal end processing cannot
     occur in printEnd so after running expat we have to finish the wrapper
     manually later in this routine */
  if (!cop->html && ((cop->echo || cop->wrap) && !cop->chunk_id))
    {
      printHTMLStart(&fragdata);
      fragdata.nesting = 1;
    }

  runexpatNSuD(i_list, fnlist, content_sH, content_eH, NULL, &fragdata);

  if (cop->html)
    fputs("</html>", fragdata.fp);
  else if ((cop->echo || cop->wrap) && !cop->chunk_id)
    fputs("</body></html>", fragdata.fp);

  return 0;
}

static char *
set_p4pager(Isp *ip)
{
  char p4pager[strlen("id=.p4Pager..data-proj=...data-item=..0")+strlen(ip->itemdata.proj)+strlen(ip->itemdata.item)];
  sprintf(p4pager, "id=\"p4Pager\" data-proj=\"%s\" data-item=\"%s\"", ip->itemdata.proj, ip->itemdata.item);
  return strdup(p4pager);
}

#if 0
static const char *
html_wild(const char *input)
{
  char *tmp = malloc(strlen(input)+2);
  char *res = NULL;
  glob_t gres;
  sprintf(tmp,"%s*",input);
  if (!glob_pattern(tmp,&gres))
    res = strdup(gres.gl_pathv[0]);
  globfree(&gres);
  return res;
}
#endif

static const char **
addClassSelect(const char **atts)
{
  int i = 0;
  char **newAtts = NULL;
  if (atts)
    {
      int class_i = -1;
      for (i = 0; atts[i]; ++i)
	;
      newAtts = malloc((((i+1)*2)+1) * sizeof(const char *));
      for (i = 0; atts[i]; i += 2)
	{
	  newAtts[i] = (char*)atts[i];
	  newAtts[i+1] = (char*)atts[i+1];
	  if (!strcmp(atts[i], "class"))
	    class_i = i;
	}
      if (class_i >= 0)
	{
	  char *newClass = malloc(strlen(atts[class_i+1]) + strlen(" selected") + 1);
	  strcpy(newClass, atts[class_i+1]);
	  strcat(newClass, " selected");
	  newAtts[class_i+1] = newClass;
	}
      else
	{
	  newAtts[i] = "class";
	  newAtts[i+1] = "selected";
	  newAtts[i+2] = NULL;
	}
    }
  else
    {
      newAtts = malloc(3 * sizeof(const char *));
      newAtts[0] = "class";
      newAtts[1] = "selected";
      newAtts[2] = NULL;
    }
  return (const char **)newAtts;
}

static void
content_eH(void *userData, const char *name)
{
  if (*name == 'r' && !strcmp(name, "rp-wrap"))
    return;

  if (((struct what_frag*)userData)->nesting)
    printEnd(userData, name);
  else
    charData_discard();
}

struct content_opts *
what_content_init(void)
{
  return calloc(1, sizeof(struct content_opts));
}

static void
content_sH(void *userData, const char *name, const char **atts)
{
  const char *xid = get_xml_id(atts);
  struct what_frag *frag = userData;

  if (*name == 'r' && !strcmp(name, "rp-wrap"))
    return;

  if (frag->nesting)
    printStart(userData, name, atts, xid);
  else if (xid && *xid && frag->xid && !strcmp(frag->xid, xid))
    {
      charData_discard();
      if (frag->cop->wrap)
	printHTMLStart(userData);
      printStart(userData, name, atts, xid);
    }
  else if (frag->cop->unwrap)
    {
      if (cued_printStart)
	{
	  cued_printStart = 0;
	  if (frag->cop->wrap)
	    printHTMLStart(userData);
	  printStart(userData, name, atts, xid);
	}
      else if (!strcmp(name, "body"))
	{
	  charData_discard();
	  cued_printStart = 1;
	}
      else if (frag->nesting)
	printStart(userData, name, atts, xid);
      else
	charData_discard();
    }
  else if (!strcmp(name, "gdf:dataset"))
    {
      printStart(userData, "gdf:dataset", atts, xid);
      need_gdf_closer = 1;
      frag->nesting = 0;
    }
  else if (frag->cop->html)
    {
      if (!strcmp(name, "head"))
	{
	  fputs("<!DOCTYPE html>\n<html xmlns=\"http://www.w3.org/1999/xhtml\">",
		frag->fp);
	  printStart(userData, name, atts, xid);
	  fputs("<meta name=\"generator\" content=\"px cop \"/>", frag->fp);
	}
      else if (!strcmp(name, "body"))
	{
	  char *p4pager = set_p4pager(frag->ip);
	  fprintf(frag->fp, "<body %s>", p4pager);
	  free(p4pager);
	  ++frag->nesting;
	}
    }
  else
    charData_discard();
}

static void
printEnd(struct what_frag *frag, const char *name)
{
  if (frag->cop->unwrap)
    {
      if (!strcmp(name, "body"))
	{
	  if (frag->cop->wrap)
	    fputs("\n</body></html>", frag->fp);	    
	  fclose(frag->fp);
	  exit(0);
	}
      else
	{
	  printText((const char *)charData_retrieve(), frag->fp);
	  fprintf(frag->fp, "</%s>", name);
	  return;
	}
    }

  printText((const char *)charData_retrieve(), frag->fp);
  fprintf(frag->fp, "</%s>", name);
  if (!--frag->nesting)
    {
      if (frag->cop->html)
	{
	  if (!strcmp(name, "head"))
	    {
	      /*
		char *p4pager = set_p4pager(frag->ip);
		fprintf(frag->fp, "<body %s>", p4pager);
		free(p4pager);
	       */
	    }
#if 0
	  else
	    fputs("</body></html>", frag->fp);
#endif
	}
      else
	{
	  if (need_gdf_closer)
	    fputs("</gdf:dataset>", frag->fp);
	  else
	    {
	      if (frag->cop->wrap)
		fputs("</body></html>", frag->fp);
	      fclose(frag->fp);
	      exit(0);
	    }
	}
    }
}

static void
printHTMLStart(struct what_frag *frag)
{
  fprintf(frag->fp,
	  "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
	  "<head>\n"
	  "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>\n"
	  "<title>%s</title>\n"
	  "<link rel=\"shortcut icon\" type=\"image/ico\" href=\"/favicon.ico\" />\n"
	  "<link rel=\"stylesheet\" type=\"text/css\" href=\"/%s/css/p3colours.css\" media=\"screen\"/>\n"
	  "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p3.css\" media=\"screen\"/>\n"
	  "<script src=\"/js/p4.js\" type=\"text/javascript\"> </script>\n"
	  "<script src=\"https://code.jquery.com/jquery-3.2.1.min.js\" type=\"text/javascript\"><![CDATA[ ]]></script>\n",
	  frag->cop->title ? frag->cop->title : frag->ip->project, frag->ip->project);
  if (frag->cop->ga)
    {
      fprintf(frag->fp,
	      "<!-- Google tag (gtag.js) -->\n"
	      "<script async=\"async\" src=\"https://www.googletagmanager.com/gtag/js?id=G-0QKC3P5HJ1\"></script>\n"
	      "<script>\n"
	      "  window.dataLayer = window.dataLayer || [];\n"
	      "  function gtag(){dataLayer.push(arguments);}\n"
	      "  gtag('js', new Date());\n"
	      "\n"
	      "  gtag('config', 'G-0QKC3P5HJ1');\n"
	      "</script>\n"
	      );
    }
  char *p4pager = set_p4pager(frag->ip);
  if (frag->cop->frag_id)
    fprintf(frag->fp, "</head>\n<body %s class=\"printHTMLStart\" onload=\"location.hash='%s'\">\n", p4pager, frag->cop->frag_id);
  else
    fprintf(frag->fp, "</head>\n<body %s>\n", p4pager);
  free(p4pager);
}

static void
printStart(struct what_frag *frag, const char *name, const char **atts, const char *xid)
{
  const char **ap = atts;
  printText((const char*)charData_retrieve(), frag->fp);
  fprintf(frag->fp, "<%s", name);
  if (xid && frag->cop->hilite_id && !strcmp(xid, frag->cop->hilite_id))
    atts = addClassSelect(atts);
  if (atts)
    {
      for (ap = atts; ap[0]; )
	{
	  if (frag->cop->sigs && *ap[0] == 'h' && !strcmp(ap[0], "href"))
	    {
	      const char *pop1sig = NULL;
	      /*fprintf(stderr, "content:printStart: found href\n");*/
	      if ((pop1sig = strstr(ap[1], "pop1sig(")))
		{
		  const char *p;

		  fprintf(frag->fp, " href=\"");
		  for (p = ap[1]; p < pop1sig; ++p)
		    fputc(*p, frag->fp);
		  for (p = pop1sig; '(' != *p; ++p)
		    fputc(*p, frag->fp);
		  fputc('(', frag->fp);
		  ++p;
		  fprintf(frag->fp, "'%s','',", frag->ip->project);
		  printText(p, frag->fp);
		  fputc('"', frag->fp);
		  ap += 2;
		  continue;
		}
	    }
	  fprintf(frag->fp, " %s=\"",*ap++);
	  printText(*ap++, frag->fp);
	  fputc('"', frag->fp);
	}
    }
  fputc('>', frag->fp);
  ++frag->nesting;
}

static void
printText(const char *s, FILE *frag_fp)
{
  while (*s)
    {
      if (*s == '<')
	fputs("&lt;",frag_fp);
      else if (*s == '&')
	fputs("&amp;",frag_fp);
      else if (*s == '"')
	fputs("&quot;",frag_fp);
      else
	fputc(*s,frag_fp);
      ++s;
    }
}
