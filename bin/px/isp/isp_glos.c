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

#if 0
      const char *s = tab;
      int len = strlen(ip->glosdata.glet);
      while (*s)
	if (!strncmp(s, ip->glosdata.glet, len) && (!s[len+1] || '\t' == s[len+1]))
	  break;
      if (*s)
	{
	  lid = (char*)pool_alloc(len+1, ip->p);
	  strncpy(lid, s, len);
	  ip->glosdata.ltab = tab;
	}
      else
	ip->err = "no such letter in letter-ids.tab";
#endif

int
isp_glos_zoom(Isp *ip)
{
  ip->cache.ltab = (ccp)pool_alloc(strlen(ip->cache.sub)+strlen("/letters.tab0"), ip->p);
  sprintf((char*)ip->cache.ltab, "%s/letters.tab", ip->cache.sub);
  FILE *lfp = fopen(ip->cache.zout, "w");
  if (lfp)
    {
      FILE *tfp = fopen(ip->cache.ltab, "w");
      if (tfp)
	{
	  /* ltab is malloc data */
	  if (!isp_glos_letter_id(ip))
	    return 1;
	  char *s = (char*)ip->glosdata.ltab;
	  fprintf(lfp, "<div id=\"p4Letters\">");
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
		{
		  fprintf(lfp, "<p><a href=\"javascript://p4letter('%s')\">%s</a></p>", let, let);
		  fprintf(tfp, "%s\t%s\n", let, lid);
		}

	      s = l;
	    }
	  fprintf(lfp,"</div>");
	  fclose(lfp);
	  fclose(tfp);
	}	      
      else
	{
	  ip->err = "isp_glos_zoom: unable to open letters.tab";
	  fclose(lfp);
	  return 1;
	}
    }
  else
    {
      ip->err = "isp_glos_zoom: unable to open letters.div";
      return 1;
    }
  return 0;
}

int
isp_glos_glid(Isp *ip)
{
  char *t = (char*)slurp("isp_glos_glid", ip->cache.ltab, NULL);
  if (!t)
    {
      ip->err = "isp_glos_glid: failed to load letters.tab";
      return 1;
    }
  int len = strlen(ip->glosdata.glet);
  char *l = t;
  while (*l)
    {
      if (!strncmp(l, ip->glosdata.glet, len) && '\t' == l[len])
	{
	  char *e;
	  l += len+1;
	  if ((e = strchr(l,'\t')))
	    *e = '\0';
	  ip->glosdata.lbase = (ccp)pool_copy((uccp)l, ip->p);
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
  free(t);
  if (!ip->glosdata.lbase)
    {
      ip->err = "isp_glos_glid: letter not fund in letters.tab";
      return 1;
    }
  return 0;
}

int
isp_glos_data(Isp *ip)
{
  if (ip->glosdata.glet)
    {
      if (isp_glos_glid(ip))
	return 1;
    }

  /* set the item-id file */
  ip->glosdata.lpath = (ccp)pool_alloc(strlen(ip->glosdata.dir)+strlen(ip->glosdata.lbase)+2, ip->p);
  sprintf((char*)ip->glosdata.lpath, "%s/%s.lst", ip->glosdata.dir, ip->glosdata.lbase);

  /* compute the ranges */
  int start = ((atoi(ip->page)-1) * atoi(ip->psiz)) + 1;
  int end = start + atoi(ip->psiz);

  /* create the page input selection */
  if (file_copy_lines(ip->glosdata.lpath, ip->cache.pgin, start, end))
    return 1;

  /* now we can use the main page creator to make the page div */
  return create_page_div(ip);
}
