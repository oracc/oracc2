#include <oraccsys.h>
#include "isp.h"

/* This is an oracc2 implementation of the oracc1 'tis' subsystem; in
   oracc2 'tis' means 'token instance set' rather than oracc1's 'text
   instance set'

   The indexes accessed by this routine are build by
   /oracc/c/bin/util/xisdb.c.  The .dbi index contains information
   structures which are used to ftell into the .tis file.
 */
struct xis_info { long int seek; int len; int count; };

static void
xis_wids(Isp *ip, const struct xis_info *xip, FILE *fp)
{
  FILE *xis_fp = NULL;
  char xis_file[strlen(ip->lloc.dbpath)+strlen(ip->lloc.lang)+3];
  sprintf(xis_file, "%s/%s.tis", ip->lloc.dbpath, ip->lloc.lang);
  if (ip->verbose)
    fprintf(stderr, "isp: xis_wids: .tis=%s\n", xis_file);
  if ((xis_fp = fopen(xis_file, "r")))
    {
      int n = 0;
      fseek(xis_fp, xip->seek, 0);
      while (n++ <= xip->len)
        {
          int ch = fgetc(xis_fp);
          if (ch == EOF)
            {
              ip->err = "isp: xis_wids: read failure getting xis ids from .tis file";
	      break;
            }
          else
            {
              if (ch == ' ')
                fputc('\n', fp);
              else
                fputc(ch, fp);
            }
        }
      fclose(xis_fp);
    }
  else
    ip->err = "unable to open .tis file for xis ids";
}

int
isp_xis_list(Isp *ip)
{
  Dbi_index *dp = dbx_init(ip->lloc.dbpath, ip->lloc.dbname);
  if (dp)
    {
      Unsigned32 len;
      const struct xis_info *xip = dbx_key(dp, ip->list_name, &len);
      if (xip)
	{
	  FILE *fp;
	  if (ip->verbose)
	    fprintf(stderr, "isp: isp_xis_list creating cache.list %s\n", ip->cache.list);
	  if (!(fp = fopen(ip->cache.list, "w")))
	    ip->err = "unable to create cache.list";
	  else
	    xis_wids(ip, xip, fp);
	}
      else
	ip->err = "key not found in xis db";
      dbx_term(dp);
    }
  else
    ip->err = "failed to open .dbh/.dbi database";
  return ip->err ? 1 : 0;
}
