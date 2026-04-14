#include <oraccsys.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <joxer.h>
#include <xml.h>
#include "form.h"
#include "ilem_form.h"
#include "links.h"
#include "xcl.h"

#define XML_DECL "<?xml version=\"1.0\" encoding=\"utf-8\"?>"

struct nsa_xcl_data;
extern void nsa_xcl_dump(struct nsa_xcl_data *);

#if 0
static const char *xlink_ns = "http://www.w3.org/1999/xlink";
static const char *xcl_ns = "http://oracc.org/ns/xcl/1.0";
static const char *xff_ns = "http://oracc.org/ns/xff/1.0";
#endif

extern int links_standalone;

static const char *
hashify(const char *ref)
{
  char buf[strlen(ref)+2];
  sprintf(buf, "#%s", ref);
  return strdup(buf);
}

void
linkset_jox(struct linkset *lsp)
{
  int i;
  const char *r[] = {
    "xl:title", lsp->title,
    "xml:id", lsp->xml_id,
    "xl:type", "extended",
    "xl:role", lsp->role,
    lsp->form.sig ? "sig" : NULL, (ccp)lsp->form.sig,
    NULL,
  };
  joxer_ea(NULL, "linkset", rnvval_aa_ccpp(r));
  for (i = 0; i < lsp->used; ++i)
    {
      List *ap = list_create(LIST_SINGLE);
      joxer_attr(ap, "xl:title",lsp->links[i].title);
      joxer_attr(ap, "xl:type", "locator");
      joxer_attr(ap, "xl:href", hashify(lsp->links[i].lref));
      joxer_attr(ap, "xl:role", lsp->links[i].role);
      if (links_standalone)
	joxer_attr(ap, "wordref", lsp->links[i].lp->ref);
      Ratts *ratts = ratts_list2ratts(ap);
      joxer_ea(NULL, "link", ratts);
      if (links_standalone)
	form_serialize_jox(&lsp->links[i].lp->f->f2);
      joxer_ee(NULL, "link");
    }

  if (lsp->user_dump_function)
    lsp->user_dump_function(lsp->user);
  
  joxer_ee(NULL, "linkset");
}

void
links_jox(struct linkbase *lbp)
{
  struct linkset *lsp;
  if (lbp && lbp->first)
    {
      joxer_ea(NULL, "linkbase",NULL);
      for (lsp = lbp->first; lsp; lsp = lsp->next)
	linkset_jox(lsp);
      joxer_ee(NULL, "linkbase");
    }
}

#if 0
void
links_serialize(FILE*fp,struct linkbase *lbp, int with_xml_decl)
{
  struct linkset *lsp;

  if (!lbp || !lbp->first)
    return;

  if (with_xml_decl)
    fprintf(fp,"%s\n",XML_DECL);
  fprintf(fp,"<linkbase xmlns=\"%s\" xmlns:xcl=\"%s\" xmlns:xl=\"%s\" xmlns:xff=\"%s\">",xcl_ns,xcl_ns,xlink_ns,xff_ns);
  for (lsp = lbp->first; lsp; lsp = lsp->next)
    linkset_dump(fp,lsp);
  fprintf(fp,"</linkbase>");
}
#endif
