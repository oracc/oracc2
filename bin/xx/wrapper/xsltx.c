#include <oraccsys.h>
#include "wrapper.h"

#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

extern int xmlLoadExtDtdDefaultValue;

static const char *wrapper_err = NULL;

Hash*sparms;

const char *invoke;
const char *infile;
const char *listfile;
const char *outfile;
const char *project;
const char *translation;

int htmlmode = 0;
int verbose = 0;

const char *
wrapper_last_err(void)
{
  return wrapper_err;
}

int
wrapper(const char *sheet, const char **params, const char *xmldoc, const char *output)
{
  static const char *last_sheet = NULL;
  static xsltStylesheetPtr cur = NULL;
  xmlDocPtr doc, res;

  if (verbose)
    {
      fprintf(stderr, "%s", sheet);
      int i;
      for (i = 0; params[i]; i += 2)
	fprintf(stderr, " %s=%s", params[i], params[i+1]);
      fprintf(stderr, " <%s >%s\n", xmldoc, output);
    }
  
  if (!sheet)
    {
      xsltFreeStylesheet(cur);
      cur = NULL;
      last_sheet = NULL;
    }

  xmlSubstituteEntitiesDefault(1);
  xmlLoadExtDtdDefaultValue = 1;

  if (!last_sheet || strcmp(last_sheet, sheet))
    {
      if (cur)
	xsltFreeStylesheet(cur);
      if (!(cur = xsltParseStylesheetFile((const xmlChar *)sheet)))
	{
#define SHEET_ERR "error parsing sheet 0"
	  char buf[strlen(sheet)+strlen(SHEET_ERR)];
	  sprintf(buf, "%s%s", SHEET_ERR, sheet);
	  wrapper_err = strdup(buf);
	  return 1;
	}
    }
  
  if (!(doc = xmlParseFile(xmldoc)))
    {
#define DOC_ERR "error parsing XML document 0"
      char buf[strlen(xmldoc)+strlen(DOC_ERR)];
      sprintf(buf, "%s%s", DOC_ERR, xmldoc);
      wrapper_err = strdup(buf);
      return 1;
    }
  
  if (!(res = xsltApplyStylesheet(cur, doc, params)))
    {
#define APPLY_ERR "error applying sheet___to  0"
      char buf[strlen(sheet)+strlen(xmldoc)+strlen(APPLY_ERR)];
      sprintf(buf, "error applying sheet %s to %s", sheet, xmldoc);
      wrapper_err = strdup(buf);
      return 1;
    }

  if (!output || !strcmp(output, "-"))
    xsltSaveResultToFile(stdout, res, cur);

  xmlFreeDoc(res);
  xmlFreeDoc(doc);

  return 0;
}

static const char **
files(void)
{
  char **f = NULL;
  if (infile)
    {
      f = malloc(2*sizeof(char*));
      f[0] = (char*)infile;
      f[1] = NULL;
    }
  else if (listfile)
    {
      f = (char **)loadfile_lines((ucp)listfile, NULL);
    }
  return (const char **)f;
}

static char *
sparm(const char *p)
{
  char buf[strlen(p)+3];
  char *ret;
  sprintf(buf, "'%s'", p);
  if (!(ret = hash_find(sparms, (ucp)buf)))
    {
      ret = strdup(buf);
      hash_add(sparms, (ucp)ret, "");
    }
  return ret;
}

static const char **
params(struct progtab *pinfo, const char *project, const char *trans)
{
  static char *p[7];
  int null = 2;
  p[0] = "project";
  p[1] = sparm(project);
  if (trans)
    {
      p[2] = "translation";
      p[3] = sparm(trans);
      null = 4;
    }
  p[null] = NULL;
  return (const char **)p;
}

static int
perfile(struct progtab *proginfo, const char *qpqx, const char *trans)
{
  char project[strlen(qpqx)+1];
  strcpy(project, qpqx);
  char *colon = strchr(project, ':');
  *colon = '\0';
  char *in = strdup(expand(NULL, qpqx, proginfo->inext));
  char *out = NULL;
  if (!outfile)
    out = strdup(expand(NULL, qpqx, proginfo->outext));
  else
    out = outfile;
  const char **parms = params(proginfo, project, trans);
  wrapper(proginfo->sheetpath, parms, in, out);
  free(in);
  if (!outfile)
    free(out);
  return 0;
}

static struct progtab *
program(const char *progarg)
{
  const char *slash = strrchr(progarg, '/');
  if (!slash)
    slash = progarg;
  else
    ++slash;
  struct progtab *p = progtab(slash, strlen(slash));
  if (p)
    {
      const char *oracc = oracc_builds();
      const char *scripts = "/lib/scripts/";
      char buf[strlen(oracc)+strlen(scripts)+strlen(p->sheet)+1];
      sprintf(buf, "%s%s%s", oracc, scripts, p->sheet);
      p->sheetpath = strdup(buf);
    }
  return p;
}

int
main(int argc, char **argv) {
  sparms = hash_create(5);

  struct progtab *proginfo = program(invoke = argv[0]);

  if (!proginfo)
    {
      fprintf(stderr, "%s: unknown invocation name for xsltx\n", argv[0]);
      return 1;
    }
  
  options(argc, argv, "hi:l:o:p:t:v");

  if (infile || listfile)
    {
      const char **f = files();
      int i;
      for (i = 0; f[i]; ++i)
	perfile(proginfo, f[i], translation);
    }
  else
    {
      const char *f;
      while ((f = (ccp)loadoneline(stdin, NULL)))
	perfile(proginfo, f, translation);
    }
  
  xsltCleanupGlobals();
  xmlCleanupParser();

  return(0);
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case 'h':
      htmlmode = 1;
      expand_base("/home/oracc/www/htm");
      break;
    case 'i':
      infile = arg;
      break;
    case 'l':
      listfile = arg;
      break;
    case 'o':
      outfile = arg;
      break;
    case 'p':
      project = arg;
      break;
    case 't':
      translation = arg;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      fprintf(stderr, "%s: unknown option -%c\n", invoke, opt);
      return 1;
    }
  return 0;
}

void help(void) { }

#if 0
#define usage xusage

static void usage(const char *name) {
    printf("Usage: %s [options] stylesheet file [file ...]\n", name);
    printf("      --param name value : pass a (parameter,value) pair\n");

}

  if (argc <= 1)
    {
      usage(argv[0]);
      return(1);
    }
  
  for (i = 1; i < argc; i++)
    {
      if (argv[i][0] != '-')
	break;
      if (!strcmp(argv[i], "-param"))
	{
	  i++;
	  params[nbparams++] = argv[i++];
	  params[nbparams++] = argv[i];
	  if (nbparams >= 16)
	    {
	      fprintf(stderr, "too many params\n");
	      return (1);
	    }
	}
      else
	{
	  fprintf(stderr, "Unknown option %s\n", argv[i]);
	  usage(argv[0]);
	  return (1);
	}
    }
  
  params[nbparams] = NULL;
#endif
  
