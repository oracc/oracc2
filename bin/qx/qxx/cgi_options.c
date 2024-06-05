#include <oraccsys.h>
#include "../../px/px.h"
#include "qx.h"

extern struct cgioptstab *cgiopts (register const char *str, register size_t len);

int
qx_cgi_options(int argc, char **argv, struct qxdata *qp)
{
  for (argc = 1; argv[argc]; ++argc)
    {
      char *opt = (char*)pool_copy((ucp)argv[argc], qp->p);
      char *equals = strchr(opt, '=');
      if (equals)
	{
	  *equals++ = '\0';
	  equals = (char*)pool_copy((ucp)CGI_decode_url(equals), qp->p);
	}
      struct cgioptstab *t = cgiopts(opt, strlen(opt));
      if (!t || opts(t->opt,equals))
	{
	  qp->err = qx_err("invalid option: %s\n", argv[argc]);
	  return 1;
	}
    }
  return 0;
}
