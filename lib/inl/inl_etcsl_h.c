#include <tree.h>
#include <prop.h>
#include "inl.h"

void
inl_etcsl_h(Scan *sp, Scanseg *ssp, Tree *tp, char *s)
{
  char *square = NULL;
  const char *tag = ((struct inltok*)ssp->user)->name;
  if (!strcmp(tag, "q") || !strcmp(tag, "xref"))
    {
      square = (char*)ssp->attr;
      ssp->attr = NULL;
    }
  inl_span_node(sp, ssp, tp, s);
  if (square)
    {
      if ('q' == *tag)
	{
	  char *comma = strchr(square,',');
	  if (comma)
	    *comma++ = '\0';
	  prop_node_add(ssp->np, INL_ATTR, PG_XML, "data-speaker", square);
	  prop_node_add(ssp->np, INL_ATTR, PG_XML, "data-addressee", comma);
	}
      else
	prop_node_add(ssp->np, INL_ATTR, PG_XML, "data-ref", square);
    }
}
