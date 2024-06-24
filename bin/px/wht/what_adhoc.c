#include <oraccsys.h>
#include "../px.h"
#include "what.h"

int
what_adhoc(Isp *ip)
{
  const char *items = ip->item;
  ip->from = "list";
  ip->item = NULL;

  char *d = (char*)pool_alloc(strlen(ip->cache.project)+strlen(ADHOCTMP)+2, ip->p);
  sprintf(d, "%s/%s", ip->cache.project, ADHOCTMP);
  char *slash = strrchr(d, '/');

  if (!(ip->srchdata.tmp = mkdtemp(d)))
    {
      ip->err = (ccp)pool_copy((ucp)px_err("failed to make adhoc tmpdir %s: %s",
					   d, strerror(errno)), ip->p);
    }
  else
    {
      ip->srchdata.list = slash+1;
      ip->srchdata.adhoc = 1;
      ip->list_name = "list";
      ip->lloc.path = (ccp)pool_alloc(strlen(ip->srchdata.tmp)+strlen("/list0"),ip->p);
      sprintf((char*)ip->lloc.path, "%s/list", ip->srchdata.tmp);
      ip->list_name = "list";
  
      FILE *fp = NULL;

      fprintf(stderr, "px: saving adhoc '%s' to %s/list\n", items, d);
      if ((fp = fopen(ip->lloc.path, "w")))
	{
	  const char *s = items;
	  while (*s)
	    {
	      if (',' == *s)
		fputc('\n', fp);
	      else
		fputc(*s, fp);
	      ++s;
	    }
	  fputc('\n', fp);
	  fclose(fp);
	}
      else
	{
	  fprintf(stderr, "qx: failed to open %s to write adhoc list\n", ip->lloc.path);
	  ip->err = (ccp)pool_copy((ucp)qx_err("failed to write items to adhoc list"), ip->p);
	}
    }

  if (!access(ip->lloc.path, R_OK))
    {
      return PX_CONTINUE;
    }
  else
    {
      /* Don't error here; let the process produce the p4pager and
	 report the issue in the content */
      ip->nowhat = 1;
      return PX_CONTINUE;
    }
  return PX_ERROR;  
}
