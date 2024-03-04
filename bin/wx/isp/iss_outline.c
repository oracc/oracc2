#include <oraccsys.h>
#include <xmlify.h>
#include "isp.h"

#if 0
extern int zoomid, xml_outline;
extern const char *outline_keys[];
#endif

int uzpage = -1;
int zprev = -1;
int znext = -1;

#if 0
const char *xo_head = 
  "<title>Outline</title>\n"
  "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p2-p1.css\"/>\n"
  "<script type=\"text/javascript\" src=\"/js/p2.js\">\n</script>\n"
  ;
#endif

void
iss_outline_dump(Isp *ip, FILE *fp)
{
  static u4 u4max = 0xffffffff;
  int i, j;
  u4 *lastlevs = malloc(ip->op_nlevels * sizeof(u4*));
  int olevs = 0;
  int nheadings = ip->op_nlevels - 1;
#if 0
  extern int fragment;
#endif

  for (i = 0; i < ip->op_nlevels; ++i)
    lastlevs[i] = u4max;

#if 0
  if (xml_outline)
    {
      extern const char *project;
      if (!fragment)
	{
	  fprintf(fp, "<html xmlns=\"http://www.w3.org/1999/xhtml\"><head>%s", xo_head);
	  fprintf(fp, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/p2.css\"/>\n");
	  fprintf(fp, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/%s/p2.css\"/>\n", project);
	  fprintf(fp, "<script src=\"/js/p2.js\" type=\"text/javascript\">\n</script>\n");
	  fprintf(fp, "<script src=\"/js/p2-shortcuts.js\" type=\"text/javascript\">\n</script>\n");
	  fprintf(fp, "<script src=\"/js/p2-keys.js\" type=\"text/javascript\">\n</script>\n");
	  fprintf(fp, "</head><body onload=\"p2Keys()\">");
	}
    }
#endif
  
  fputs("<div class=\"pgotl level0\">",fp);
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
		  uzpage = o[i].page;
		  if (i)
		    zprev = o[i-1].sic->id;
		  if (i < sic_size-1)
		    znext = o[i+1].sic->id;
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
#if 0
		      if (outline_keys[j+1])
			{
			  
			}
		      else
			{
#endif
			  fprintf(fp, "<h%d>%c%s</h%d>",j+2,
				  toupper(sip->pool[o[i].poffsets[j]]),
				  (char*)xmlify(&sip->pool[o[i].poffsets[j]+1]),
				  j+2);
#if 0
			}
#endif
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
#if 0
  if (xml_outline && !fragment)
    fputs("</body></html>",fp);
#endif
}
