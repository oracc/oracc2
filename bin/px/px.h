#ifndef PX_H_
#define PX_H_

#include "pxdefs.h"

#include <stdlib.h>
#include "xslt_if.h"
#include "isp/isp.h"
#include "pui/pui.h"

struct cgioptstab
{
  const char *name;
  int opt;
};

struct frag
{
  int nesting;
  const char *xid;
  struct xft *xp;
  int xp_on;
  FILE *fp;
};

#include "xftable.h"

#include "px.proto"
extern int px_return(Isp *ip);
extern struct cgioptstab *cgiopts(register const char *str, register size_t len);
extern struct ispargstab *pxargs (register const char *str, size_t len);
#endif/*PX_H*/
