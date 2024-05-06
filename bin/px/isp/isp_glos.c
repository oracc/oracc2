#include <oraccsys.h>
#include "isp.h"

const char *
isp_glos_letter_id(Isp *ip)
{
  char fn[strlen(ip->glosdata.dir)+strlen("/letter_ids.tab0")];
  sprintf(fn, "%s/letter_ids.tab", ip->glosdata.dir);
  const char *tab = (ccp)slurp("isp_glos_letter_id", fn, NULL);
  if (!tab)
    ip->err = "unable to load letter_ids.tab";
  else
    ip->glosdata.ltab = tab;
  return tab;
}

int
isp_glos_list(Isp *ip)
{
  /* ltab is malloc data */
  if (!isp_glos_letter_id(ip))
    return 1;

  if (ip->zoom)
    {
      ip->glosdata.glet = ip->zoom;
      if (isp_glos_glid(ip))
	return 1;
    }
  else
    {
      ip->glosdata.lbase = "entry_ids";
    }

  char p[strlen(ip->oracc)+strlen("/pub/")+strlen(ip->project)+strlen(ip->glosdata.lbase)+4];
  sprintf(p, "%s/pub/%s/cbd/%s/%s.lst", ip->oracc, ip->project, ip->glos, ip->glosdata.lbase);
  ip->lloc.path = ip->glosdata.lpath = (ccp)pool_copy((uccp)p, ip->p);
  ip->lloc.method = "file";
  ip->cemd = "cglo";
  ip->data = "dglo";

  return 0;
}

int
isp_glos_menu(Isp *ip)
{
  FILE *lfp = fopen(ip->cache.zout, "w");
  if (lfp)
    {
      /* We always use pub/cbd/LANG/letters.tab for the letter-id;
       *
       * We should only need to create ip->cache.zout once, but right now we recreate it every time
       */
      char *s = (char*)ip->glosdata.ltab;
      fprintf(lfp, "<div id=\"p4Letters\">");
      fprintf(lfp, "<p class=\"all-letters\"><a href=\"javascript://\" onclick=\"act_letter('entry_ids')\">ALL</a></p>");
      while (*s)
	{
	  char *l = s;
	  char *let = s, *lid;
	      
	  while (*l && '\t' != *l)
	    {
	      if ('\'' == *l)
		*l = (char)31;
	      ++l;
	    }
	  if (*l)
	    *l++ = '\0';
	      
	  lid = l;
	  while (*l && '\t' != *l)
	    ++l;

	  if (*l)
	    *l++ = '\0';
	  if (*let && !isspace(*let))
	    fprintf(lfp, "<p><a href=\"javascript://\" onclick=\"act_letter('%s')\">%s</a></p>", let, let);

	  s = l;
	}
      fprintf(lfp,"</div>");
      fclose(lfp);
    }
  else
    {
      ip->err = "isp_glos_menu: unable to open letters.div";
      return 1;
    }
  return 0;
}

int
isp_glos_glid(Isp *ip)
{
  if (ip->glosdata.glet && !strcmp(ip->glosdata.glet, "entry_ids"))
    {
      ip->glosdata.lbase = ip->glosdata.glet;
    }
  else
    {
      int len = strlen(ip->glosdata.glet);
      char *l = (char*)ip->glosdata.ltab;
      while (*l)
	{
	  if (!strncmp(l, ip->glosdata.glet, len) && '\t' == l[len])
	    {
	      char *e, save = '\0';
	      l += len+1;
	      if ((e = strchr(l,'\t')))
		{
		  save = '\t';
		  *e = '\0';
		}
	      ip->glosdata.lbase = (ccp)pool_copy((uccp)l, ip->p);
	      if (save)
		*e = '\t';
	      break;
	    }
	  else
	    {
	      /* Skip the letter */
	      while (*l && '\t' != *l)
		++l;
	      ++l;
	      /* Skip the ID */
	      while (*l && '\t' != *l)
		++l;
	      if ('\t' == *l)
		++l;
	      /* Now we are teed up at the next letter */
	    }
	}
      if (!ip->glosdata.lbase)
	{
	  ip->err = "isp_glos_glid: letter not found in letters.tab";
	  return 1;
	}
    }
  return 0;
}

int
isp_glos_data(Isp *ip)
{
  /* set the letter-id file */
  ip->glosdata.lpath = (ccp)pool_alloc(strlen(ip->glosdata.dir)+strlen(ip->glosdata.lbase)+2, ip->p);
  sprintf((char*)ip->glosdata.lpath, "%s/%s.lst", ip->glosdata.dir, ip->glosdata.lbase);

  /* compute the ranges */
  int start = ((atoi(ip->page)-1) * atoi(ip->psiz)) + 1;
  int end = start + atoi(ip->psiz);

  /* create the page input selection */
  int ret;
  if ((ret = file_copy_lines(ip->glosdata.lpath, ip->cache.pgin, start, end)))
    {
      if (ret == 1)
	{
	  ip->err = PX_ERROR_START "file_copy_lines: failed to open input %s\n";
	  ip->errx = ip->glosdata.lpath;
	  return 1;
	}
      else if (ret == 2)
	{
	  ip->err = PX_ERROR_START "file_copy_lines: failed to open output %s\n";
	  ip->errx = ip->cache.pgin;
	  return 1;
	}
      else if (ret == 3)
	{
	  ip->err = "file_copy_lines: error copying lines";
	  return 1;
	}
    }

  /* now we can use the main page creator to make the page div */
  return create_page_div(ip);
}
