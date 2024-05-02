#include <oraccsys.h>
#include <xslt_if.h>

extern int xslt_parse_xslmem(Xslt *xp);

static const char *xslt_if_err = NULL;

const char *
xslt_if_lasterr(void)
{
  return xslt_if_err;
}

void
xslt_init(void)
{
  exsltRegisterAll();
}

void
xslt_term(Xslt *xp)
{
  xsltFreeStylesheet(xp->cur);
  xmlFreeDoc(xp->doc);
  xmlFreeDoc(xp->res);
  xsltCleanupGlobals();
  xmlCleanupParser();
}

int
xslt_parse_sheet(Xslt *xp)
{
  if (xp->xslstr)
    {
      if (xslt_parse_xslmem(xp))
	return 1;
      
      if (!(xp->cur = xsltParseStylesheetDoc(xp->xslstrdoc)))
	{
#define STRSHEET_ERR "error parsing sheet "
	  char buf[strlen(xp->xsl)+strlen(STRSHEET_ERR)+1];
	  sprintf(buf, "%s%s", STRSHEET_ERR, xp->xsl);
	  xslt_if_err = strdup(buf);
	  return 1;
	}
    }
  else if (!(xp->cur = xsltParseStylesheetFile((const xmlChar *)xp->xsl)))
    {
#define SHEET_ERR "error parsing sheet "
      char buf[strlen(xp->xsl)+strlen(SHEET_ERR)+1];
      sprintf(buf, "%s%s", SHEET_ERR, xp->xsl);
      xslt_if_err = strdup(buf);
      return 1;
    }
  return 0;
}

int
xslt_parse_doc(Xslt *xp)
{
  if (xp->xmlstr)
    {
      return xslt_parse_docmem(xp);
    }
  else if (!(xp->doc = xmlParseFile(xp->xml)))
    {
#define DOC_ERR "error parsing XML document "
      char buf[strlen(xp->xml)+strlen(DOC_ERR)+1];
      sprintf(buf, "%s%s", DOC_ERR, xp->xml);
      xslt_if_err = strdup(buf);
      return 1;
    }
  return 0;
}

int
xslt_parse_docmem(Xslt *xp)
{
  if (!(xp->doc = xmlParseMemory(xp->xmlstr, strlen(xp->xmlstr))))
    {
#define XMLMEMDOC_ERR "error parsing XML string "
      char buf[strlen(xp->xml)+strlen(XMLMEMDOC_ERR)+1];
      sprintf(buf, "%s%s", XMLMEMDOC_ERR, xp->xml);
      xslt_if_err = strdup(buf);
      return 1;
    }
  return 0;
}

int
xslt_parse_xslmem(Xslt *xp)
{
  if (!(xp->xslstrdoc = xmlParseMemory(xp->xslstr, strlen(xp->xslstr))))
    {
#define XSLMEMDOC_ERR "error parsing XML string "
      char buf[strlen(xp->xsl)+strlen(XSLMEMDOC_ERR)+1];
      sprintf(buf, "%s%s", XSLMEMDOC_ERR, xp->xsl);
      xslt_if_err = strdup(buf);
      return 1;
    }
  return 0;
}

int
xslt_transform(Xslt *xp)
{
  if (!(xp->res = xsltApplyStylesheet(xp->cur, xp->doc, xp->params)))
    {
#define APPLY_ERR "error applying sheet___to 0"
      char buf[strlen(xp->xsl)+strlen(xp->xml)+strlen(APPLY_ERR)];
      sprintf(buf, "error applying sheet %s to %s", xp->xsl, xp->xml);
      xslt_if_err = strdup(buf);
      return 1;
    }
  return 0;
}

int
xslt_output(Xslt *xp)
{
  FILE *outfp;
  
  if (!xp->out || !strcmp(xp->out, "-"))
    outfp = stdout;
  else if (!(outfp = fopen(xp->out, "w")))
    {
#define OUT_ERR "unable to open XML output "
      char buf[strlen(xp->out)+strlen(OUT_ERR)+1];
      sprintf(buf, "%s%s", OUT_ERR, xp->out);
      xslt_if_err = strdup(buf);
      return 1;
    }
  int wbytes = xsltSaveResultToFile(outfp, xp->res, xp->cur);
  if (strcmp(xp->out, "-"))
    fclose(outfp);
  if (wbytes < 0)
    {
#define WR_ERR "error writing XML output "
      char buf[strlen(xp->out)+strlen(WR_ERR)+1];
      sprintf(buf, "%s%s", WR_ERR, xp->out);
      xslt_if_err = strdup(buf);
      return 1;
    }
  return 0;
}

Xslt *
xslt_one_off(const char *xml, const char *xmlstr,
	     const char *xsl, const char *xslstr,
	     const  char *out, const char **params)
{
  extern int verbose;

  Xslt xt = {
    .xml=xml, .xmlstr=xmlstr,
    .xsl=xsl, .xslstr=xslstr,
    .out=out,
    .params=params
  };
  
  if (verbose)
    {
      fprintf(stderr, "%s", xt.xsl);
      int i;
      for (i = 0; params[i]; i += 2)
	fprintf(stderr, " %s=%s", params[i], params[i+1]);
      fprintf(stderr, " <%s >%s\n", xt.xml, xt.out);
    }

  if (xslt_parse_sheet(&xt))
    return NULL;
  if (xslt_parse_doc(&xt))
    return NULL;
  if (xslt_transform(&xt))
    return NULL;
  if (xt.out && xslt_output(&xt))
    return NULL;

  Xslt *xp = malloc(sizeof(Xslt));
  *xp = xt; 
  return xp;
}
