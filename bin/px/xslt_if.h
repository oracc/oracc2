#ifndef XSLT_IF_
#define XSLT_IF_

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
#include <libexslt/exslt.h>

typedef struct Xslt
{
  const char *xml;
  const char *xsl;
  const char *out;
  xsltStylesheetPtr cur;
  xmlDocPtr doc;
  xmlDocPtr res;
  const char **params;
} Xslt;

extern const char * xslt_if_lasterr(void);
extern void xslt_init(void);
extern void xslt_term(Xslt *xp);
extern int xslt_parse_sheet(Xslt *xp);
extern int xslt_parse_doc(Xslt *xp);
extern int xslt_transform(Xslt *xp);
extern int xslt_output(Xslt *xp);
extern Xslt* xslt_one_off(const char *xml, const char *xsl, const  char *out, const char **params);

#endif/*XSLT_IF_*/

