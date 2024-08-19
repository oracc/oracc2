#include <oraccsys.h>
#include "isp.h"

int
isp_tis_list(Isp *ip)
{
  Dbi_index *dp = dbx_init(ip->lloc.dbpath, ip->lloc.dbname);
  if (dp)
    {
      Unsigned32 len;
      dp->h.sep_char = '\n';
      const Loc8*l8p = dbx_key(dp, ip->lloc.key, &len);
      if (l8p)
	{
	  FILE *fp;
	  if (ip->verbose)
	    fprintf(stderr, "isp: isp_tis_list: creating cache.list %s\n", ip->cache.list);
	  if (!(fp = fopen(ip->cache.list, "w")))
	    {
	      ip->err = PX_ERROR_START "unable to create cache.list %s\n";
	      ip->errx = ip->cache.list;
	    }	  
	  else
	    {
	      dbx_wids(dp, l8p, len, fp);
	      fclose(fp);
	    }
	}
      else
	ip->err = "fatal: key not found in tis db";
      dbx_term(dp);
    }
  else
    ip->err = "fatal: failed to open .dbh/.dbi database";
  return ip->err ? 1 : 0;
}
