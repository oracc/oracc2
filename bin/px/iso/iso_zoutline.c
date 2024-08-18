#include <oraccsys.h>
#include "iso.h"

static void iso_zout_parse(unsigned char *lp, struct isoz *iop);
static void iso_outline_p(Isp *ip, FILE *fp, unsigned char *h, int count, int zoom, int zoomed);

int
iso_zoutline(Isp *ip)
{
  static struct iso isos;
  
  memset(&isos, '\0', sizeof(struct iso));

  const char *mol = ip->cache.mol;
  if (ip->item && ip->itemdata.dotted && (ip->srchdata.tmp || ip->glosdata.xis))
    mol = ip->cache.t_mol;

  if (!(isos.zlines = px_loadfile_lines3((uccp)mol, &isos.zmax, NULL)))
    {
      ip->err = (ccp)px_loadfile_error();
      return 1;
    }
  
  int zmax = atoi((ccp)&isos.zlines[0][1]);
  int zlev = atoi(strchr((ccp)isos.zlines[0],'/')+1);
  int zoomed = atoi(ip->zoom);

  FILE *fp;
  if (!(fp = fopen(ip->cache.zout, "w")))
    {
      ip->err = PX_ERROR_START "unable to write to zoomed outline file %s\n";
      ip->errx = ip->cache.zout;
      return 1;
    }
  fprintf(fp, "<div id=\"p4MenuOutline\" class=\"pgotl level0\" data-zmax=\"%d\">", zmax);
  int i;
  int h1_open = 0, h2_open = 0;
  int zoom = 0;
  for (i = 1; i < isos.zmax; ++i)
    {
      struct isoz iz = { NULL , NULL , NULL , 0 };
      iso_zout_parse(isos.zlines[i], &iz);
      if (iz.h1 && *iz.h1)
	{
	  if (h2_open)
	    {
	      fputs("</div>", fp);
	      h2_open = 0;
	    }
	  if (h1_open)
	    {
	      fputs("</div>", fp);
	      h1_open = 0;
	    }
	  if (zlev == 1)
	    {
	      iso_outline_p(ip, fp, iz.h1, iz.count, ++zoom, zoomed);
	    }
	  else
	    {
	      fprintf(fp, "<h2>%s</h2>", iz.h1);
	      h1_open = 1;
	      fputs("<div class=\"pgotl level1\">", fp);
	    }
	}
      if (iz.h2 && *iz.h2)
	{
	  if (zlev == 2)
	    {
	      iso_outline_p(ip, fp, iz.h2, iz.count, ++zoom, zoomed);
	    }
	  else
	    {
	      if (h2_open)
		{
		  fputs("</div>", fp);
		  h2_open = 0;
		}
	      fprintf(fp, "<h3>%s</h3>", iz.h2);
	      h2_open = 1;
	      fputs("<div class=\"pgotl level2\">", fp);
	    }
	}
      if (iz.h3 && *iz.h3)
	iso_outline_p(ip, fp, iz.h3, iz.count, ++zoom, zoomed);
    }
  if (h2_open)
    {
      fputs("</div>", fp);
      h2_open = 0;
    }
  if (h1_open)
    {
      fputs("</div>", fp);
      h1_open = 0;
    }
  fputs("</div>", fp);
  fclose(fp);
  return 0;
}

static void
iso_outline_p(Isp *ip, FILE *fp, unsigned char *h, int count, int zoom, int zoomed)
{
  const char *class;
  if (zoom == zoomed)
    class = " class=\"zoomed\"";
  else
    class = "";
  fprintf(fp, "<p><a href=\"javascript://\" onclick=\"act_zoom(%d)\" data-zoom=\"%d\"%s>%s [%d]</a></p>",
	  zoom, zoom, class, h, count);
}

static void
iso_zout_parse(unsigned char *lp, struct isoz *iop)
{
  iop->h1 = lp;
  while (*lp && '\t' != *lp)
    ++lp;
  if (*lp)
    {
      *lp++ = '\0';
      if ('#' == *lp)
	{
	  iop->count = atoi((ccp)++lp);
	}
      else
	{
	  iop->h2 = lp;
	  while (*lp && '\t' != *lp)
	    ++lp;
	  if (*lp)
	    {
	      *lp++ = '\0';
	      if ('#' == *lp)
		{
		  iop->count = atoi((ccp)++lp);
		}
	      else
		{
		  iop->h3 = lp;
		  while (*lp && '\t' != *lp)
		    ++lp;
		  if (*lp)
		    {
		      *lp++ = '\0';
		      iop->count = atoi((ccp)++lp);
		    }
		}
	    }
	}
    }
}
