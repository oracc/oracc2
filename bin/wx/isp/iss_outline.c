#include <oraccsys.h>
#include <xmlify.h>
#include "isp.h"

#if 0
int uzpage = -1;
int zprev = -1;
int znext = -1;
#endif

int
iss_outline_dump(Isp *ip)
{
  static u4 u4max = 0xffffffff;
  int i, j;
  u4 *lastlevs = malloc(ip->op_nlevels * sizeof(u4*));
  int olevs = 0;
  int nheadings = ip->op_nlevels - 1;

  for (i = 0; i < ip->op_nlevels; ++i)
    lastlevs[i] = u4max;

  char buf[strlen(ip->cache.sub)+strlen(ip->zoom)+strlen("-123-z.otl0")];
  sprintf(buf, "%s/%s-z%s.otl", ip->cache.sub, ip->perm, ip->zoom);
  ip->cache.zout = (ccp)pool_copy((uccp)buf, ip->p);

  if (!access(ip->cache.zout, W_OK))
    return 0;
  else if (!access(ip->cache.zout, F_OK))
    {
      ip->err = "is_outline_dump: output file exists but is not writable";
      return 1;
    }
  
  FILE *fp = fopen(ip->cache.zout, "w");

  if (!fp)
    {
      ip->err = "unable to write outline file";
      return 1;
    }

  if (ip->verbose)
    fprintf(stderr, "isp: iss_outline_dump: writing %s\n", ip->cache.zout);
  
  fputs("<div id=\"ispMenu\" class=\"pgotl level0\">",fp);
  struct outline *o = ip->op;
  for (i = 0; i < sic_size; ++i)
    {
      for (j = 0; ; ++j)
	{
	  const char *zoomstr = "";
	  if (j >= ip->op_nlevels)
	    break;

	  if (o[i].poffsets[j] != lastlevs[j])
	    {
	      if (o[i].sic->id == atoi(ip->zoom))
		{
		  zoomstr = " class=\"zoomed\"";
#if 0
		  uzpage = o[i].page;
		  if (i)
		    zprev = o[i-1].sic->id;
		  if (i < sic_size-1)
		    znext = o[i+1].sic->id;
#endif
		}
	      lastlevs[j] = o[i].poffsets[j];
	      
	      if (j < nheadings)
		{
		  while (olevs > j)
		    fputs("</div>",fp), --olevs;
		  fprintf(fp, "<h%d>%c%s</h%d>",j+2,
			  toupper(sip->pool[o[i].poffsets[j]]),
			  (char*)xmlify(&sip->pool[o[i].poffsets[j]+1]),
			  j+2);
		  fprintf(fp,"<div class=\"pgotl level%d\">",++olevs);
		}
	      else
		{
		  fprintf(fp, "<p%s><a href=\"javascript://\" data-zoom=\"%d\" onclick=\"p3zoom(%d)\">%s [%ld]</a></p>", 
			  zoomstr,
			  o[i].sic->id + 1,
			  o[i].sic->id + 1,
			  (char*)xmlify(&sip->pool[o[i].poffsets[j]]),
			  (long)o[i].count);
		}
	      ++j;
	      while (j < ip->op_nlevels)
		{
		  lastlevs[j] = o[i].poffsets[j];

		  if (j < nheadings)
		    {
		      while (olevs > j)
			fputs("</div>",fp), --olevs;
		      fprintf(fp, "<h%d>%c%s</h%d>",j+2,
			      toupper(sip->pool[o[i].poffsets[j]]),
			      (char*)xmlify(&sip->pool[o[i].poffsets[j]+1]),
			      j+2);
		      fprintf(fp,"<div class=\"pgotl level%d\">",++olevs);
		    }
		  else
		    {
		      fprintf(fp, "<p%s><a href=\"javascript://\" onclick=\"p3zoom(%d)\">%s [%ld]</a></p>", 
			      zoomstr,
			      o[i].sic->id + 1,
			      (char*)xmlify(&sip->pool[o[i].poffsets[j]]),
			      (long)o[i].count);
		    }
		  ++j;
		}
	    }
	}
    }
  while (olevs-- > 0)
    fputs("</div>",fp);
  fputs("</div>\n",fp);

  fclose(fp);
  
  return 0;
}
