#include <oraccsys.h>
#include "isp.h"

void
ispo_parse_page(unsigned char *lp, struct ispz *iop)
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

void
ispo_tabbed(const char *fn, FILE *fp)
{
  struct ispo *is = calloc(1, sizeof(struct ispo));
  
  is->zlines = loadfile_lines((uccp)fn, &is->zmax);
  unsigned char *h1 = NULL, *h2 = NULL;
  int i;
  fprintf(fp, "#%ld\n", is->zmax);
  for (i = 0; i < is->zmax; ++i)
    {
      struct ispz iz = { NULL , NULL , NULL , NULL, 0 };
      ispo_parse_page(is->zlines[i], &iz);
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
      if (iz.h3)
	{
	  if (iz.h1 && *iz.h1)
	    fprintf(fp, "%s\n", iz.h1);
	  if (iz.h2 && *iz.h2)
	    fprintf(fp, "\t%s\n", iz.h2);
	  fprintf(fp, "\t\t%s\t%d\n", iz.h3, iz.count);
	}
      else if (iz.h2)
	{
	  if (iz.h1 && *iz.h1)
	    fprintf(fp, "%s\n", iz.h1);
	  fprintf(fp, "\t%s\t%d\n", iz.h2, iz.count);
	}
      else
	fprintf(fp, "%s\t%d\n", iz.h1, iz.count);
    }
}

void
ispo_parse_tabbed(unsigned char *lp, struct ispz *iop)
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

void
ispo_outline_p(FILE *fp, unsigned char *h, int count, int zoom, int zoomed)
{
  const char *class;
  if (zoom == zoomed)
    class = " class=\"zoomed\"";
  else
    class = "";
  fprintf(fp, "<p%s><a href=\"javascript://\" onclick=\"p3zoom(%d)\" data-zoom=\"%d\">%s [%d]</a></p>",
	  class, zoom, zoom, h, count);
}

void
ispo_outline(const char *fn, FILE *fp, int zoomed)
{
  struct ispo *is = calloc(1, sizeof(struct ispo));
  
  is->zlines = loadfile_lines((uccp)fn, &is->zmax);
  int zmax = atoi((ccp)&is->zlines[0][1]);
  int zlev = atoi(strchr((ccp)is->zlines[0],'/')+1);
  fprintf(fp, "<div data-zmax=\"%d\">", zmax);
  int i;
  int h1_open = 0, h2_open = 0;
  int zoom = 0;
  for (i = 1; i < is->zmax; ++i)
    {
      struct ispz iz = { NULL , NULL , NULL , NULL, 0 };
      ispo_parse_tabbed(is->zlines[i], &iz);
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
	      ispo_outline_p(fp, iz.h1, iz.count, ++zoom, zoomed);
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
	      ispo_outline_p(fp, iz.h2, iz.count, ++zoom, zoomed);
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
	ispo_outline_p(fp, iz.h3, iz.count, ++zoom, zoomed);
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
}

int
main(int argc, char **argv)
{
  /*ispo_tabbed("/home/oracc/www/is.d/gudea/sux.r00002d/sort-123", stdout);*/
  ispo_outline("sort-123.tab", stdout, 2);
}
