#include <oraccsys.h>
#include "../px.h"
#include "what.h"

int
what_text(Isp *ip)
{
  if (!ip->form)
    ip->form = "html";
  
  struct whatfuncstab *w = whatfuncs(ip->form, strlen(ip->form));
  if (w)
    return w->func(ip);
  else
    {
      ip->err = px_err("fatal: unknown form request '%s' when what = '%s'\n", ip->form, ip->what);
      return PX_ERROR;
    }
}
