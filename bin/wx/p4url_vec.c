#include <oraccsys.h>
#include "px.h"
#include "p4url.h"

/* When this routine is called vl always has at least one member: project */
char const**
p4url_vec(P4url *p)
{
  char const** vec;

  int dst = 0;
  vec = malloc((p->nargs+6) * sizeof(const char*));
  
  vec[dst++] = px_exe;
  vec[dst++] = px_cgi_arg("web","1");

  vec[dst++] = px_cgi_arg("proj", p->project);
  if (p->glossary)
    vec[dst++] = px_cgi_arg("glos", p->glossary);

  if (p->oxid)
    vec[dst++] = px_cgi_arg("oxid", p->oxid);
  else if (p->pxid)
    vec[dst++] = px_cgi_arg("pxid", p->pxid);
  
  int src;

  if (p->args)
    for (src = 0; p->args[src].option; ++src)
      ((char const**)vec)[dst++] = px_cgi_arg(p->args[src].option, p->args[src].value);

  ((char **)vec)[dst] = NULL;
    
  return vec;
}
