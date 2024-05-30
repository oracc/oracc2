#include <oraccsys.h>
#include "../px.h"
#include "isp.h"

static void ispo_master_page(unsigned char *lp, struct ispz *iop);

static const char *
hclean(unsigned char *s)
{
  static char *p = NULL;
  if (!s && p)
    {
      free(p);
      return NULL;
    }

  p = realloc(p, strlen((ccp)s)+1);
  strcpy(p, (ccp)s);
  char *t = p;
  while (*t)
    if ('_' == *t)
      *t++ = ' ';
    else
      ++t;

  return p;
}

int
ispo_master(Isp *ip)
{
  if (ip->cache.sort)
    {
      unsigned char *h1 = NULL, *h2 = NULL;
      int i;

      char buf[strlen(ip->cache.sort)+strlen(".mol0")];
      sprintf(buf, "%s.mol", ip->cache.sort);
      ip->cache.mol = (ccp)pool_copy((uccp)buf, ip->p);
      if (!access(ip->cache.mol, F_OK))
	{
	  if (access(ip->cache.mol, W_OK))
	    {
	      ip->err = PX_ERROR_START "master outline %s not writeable\n";
	      ip->errx = ip->cache.mol;
	      return 1;
	    }
	}

      FILE *fp;
      if (!(fp = fopen(ip->cache.mol, "w")))
	{
	  ip->err = PX_ERROR_START "unable to open master outline %s for write\n";
	  ip->errx = ip->cache.mol;
	  return 1;
	}	

      if (!(ip->is.zlines = px_loadfile_lines3((uccp)ip->cache.sort, &ip->is.zmax, NULL)))
	{
	  ip->err =(ccp) px_loadfile_error();
	  return 1;
	}
      
      fprintf(fp, "#%ld/%d\n", ip->is.zmax, ip->is.zlev);
      for (i = 0; i < ip->is.zmax; ++i)
	{
	  struct ispz iz = { NULL , NULL , NULL , NULL, 0 };
	  ispo_master_page(ip->is.zlines[i], &iz);
	  if (h1 && !strcmp((ccp)iz.h1, (ccp)h1))
	    iz.h1 = (ucp)"";
	  else
	    h1 = iz.h1;
	  if (h2 && iz.h2 && !strcmp((ccp)iz.h2, (ccp)h2))
	    iz.h2 = (ucp)"";
	  else
	    h2 = iz.h2;
	  if (!iz.h3)
	    iz.h3 = (ucp)"";
	  /*ispo_h_clean(&iz);*/
	  if (iz.h3 && *iz.h3)
	    {
	      if (iz.h1 && *iz.h1)
		fprintf(fp, "%s\n", hclean(iz.h1));
	      if (iz.h2 && *iz.h2)
		fprintf(fp, "\t%s\n", hclean(iz.h2));
	      fprintf(fp, "\t\t%s\t#%d\n", hclean(iz.h3), iz.count);
	    }
	  else if (iz.h2 && *iz.h2)
	    {
	      if (iz.h1 && *iz.h1)
		fprintf(fp, "%s\n", hclean(iz.h1));
	      fprintf(fp, "\t%s\t#%d\n", hclean(iz.h2), iz.count);
	    }
	  else
	    fprintf(fp, "%s\t#%d\n", hclean(iz.h1), iz.count);
	}
      fclose(fp);
      (void)hclean(NULL);
    }
  return 0;
}

static void
ispo_master_page(unsigned char *lp, struct ispz *iop)
{
  iop->h1 = ++lp;
  while (*lp && !isspace(*lp))
    ++lp;
  if ('\t' == *lp)
    {
      *lp++ = '\0';
      iop->h2 = lp;
      while (*lp && !isspace(*lp))
	++lp;
      if ('\t' == *lp)
	{
	  *lp++ = '\0';
	  iop->h3 = lp;
	  while (*lp && !isspace(*lp))
	    ++lp;
	}
    }
  if (' ' == *lp)
    {
      *lp++ = '\0';
      iop->items = lp;
      int i = 1;
      while (*lp)
	{
	  if (' ' == *lp)
	    ++i;
	  ++lp;
	}
      iop->count = i;
    }
}

static void
ispo_zout_parse(unsigned char *lp, struct ispz *iop)
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
#if 0
  if (iop->h1) un_uscore(iop->h1);
  if (iop->h2) un_uscore(iop->h2);
  if (iop->h3) un_uscore(iop->h3);
#endif
}

static void
ispo_outline_p(Isp *ip, FILE *fp, unsigned char *h, int count, int zoom, int zoomed)
{
  const char *class;
  if (zoom == zoomed)
    class = " class=\"zoomed\"";
  else
    class = "";
  fprintf(fp, "<p><a href=\"javascript://\" onclick=\"act_zoom(%d)\" data-zoom=\"%d\"%s>%s [%d]</a></p>",
	  zoom, zoom, class, h, count);
}

int
ispo_zoutline(Isp *ip)
{
  if (!(ip->is.zlines = px_loadfile_lines3((uccp)ip->cache.mol, &ip->is.zmax, NULL)))
    {
      ip->err = (ccp)px_loadfile_error();
      return 1;
    }
  
  int zmax = atoi((ccp)&ip->is.zlines[0][1]);
  int zlev = atoi(strchr((ccp)ip->is.zlines[0],'/')+1);
  int zoomed = atoi(ip->zoom);

  FILE *fp;
  if (!(fp = fopen(ip->cache.zout, "w")))
    {
      ip->err = PX_ERROR_START "unable to write to zoomed outline file %s\n";
      ip->errx = ip->cache.zout;
    }
  fprintf(fp, "<div id=\"p4MenuOutline\" class=\"pgotl level0\" data-zmax=\"%d\">", zmax);
  int i;
  int h1_open = 0, h2_open = 0;
  int zoom = 0;
  for (i = 1; i < ip->is.zmax; ++i)
    {
      struct ispz iz = { NULL , NULL , NULL , NULL, 0 };
      ispo_zout_parse(ip->is.zlines[i], &iz);
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
	      ispo_outline_p(ip, fp, iz.h1, iz.count, ++zoom, zoomed);
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
	      ispo_outline_p(ip, fp, iz.h2, iz.count, ++zoom, zoomed);
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
	ispo_outline_p(ip, fp, iz.h3, iz.count, ++zoom, zoomed);
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
