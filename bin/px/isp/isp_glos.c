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
isp_glos_pmax(Isp *ip)
{
#define ENTRYCOUNTS "/entry-counts.tab"
  ip->glosdata.ecpath = (ccp)pool_alloc(strlen(ip->glosdata.dir)+strlen(ENTRYCOUNTS)+1, ip->p);
  sprintf((char*)ip->glosdata.ecpath, "%s%s", ip->glosdata.dir, ENTRYCOUNTS);
  char *ln;
  FILE *fp;
  const char *lid = ip->glosdata.lbase;
  int status = 0;
  if ((fp = fopen(ip->glosdata.ecpath, "r")))
    {
      while ((ln = (char*)loadoneline(fp, NULL)))
	{
	  if (!strncmp(ln, lid, strlen(lid)))
	    break;
	}
      fclose(fp);
      if (ln)
	{
	  char *count = strchr(ln, '\t');
	  if (count)
	    {
	      ip->glosdata.emax = (ccp)pool_copy((ucp)++count, ip->p);
	    }
	  else
	    {
	      ip->err = PX_ERROR_START "isp_glos_pmax: letter ID %s has no count\n";
	      ip->errx = lid;
	      status = 1;
	    }
	}
      else
	{
	  ip->err = PX_ERROR_START "isp_glos_pmax: letter ID %s not found in entry-counts\n";
	  ip->errx = lid;
	  status = 1;
	}
    }
  else
    {
      ip->err = PX_ERROR_START "isp_glos_pmax: failed to read %s\n";
      ip->errx = ip->glosdata.ecpath;
      status = 1;
    }

  loadoneline(NULL, NULL);
  return status;
}

/* Reset cache.sub to be the glossary cache + the gxis name

   Then set lloc so it reflects the gxis instead of the glossary entries.
 */
int
isp_glos_gxis(Isp *ip)
{
  const char *gxis_base = ip->glosdata.xis;
  char *xcache = (char*)pool_alloc(strlen(ip->cache.sub)+strlen(gxis_base)+2, ip->p);
  sprintf(xcache, "%s/%s", ip->cache.sub, gxis_base);
  struct stat sb;
  if (stat(xcache, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
      if (mkdir(xcache, 0775))
	{
	  ip->err = PX_ERROR_START "cache.sub.gxis %s could not be created\n";
	  ip->errx = xcache;
	  return 1;
	}
    }
  ip->cache.sub = xcache;
  ip->list_name = ip->glosdata.xis;
  if (isp_list_method(ip))
    return 1;
  
  ip->cemd = "line";
  ip->show = "rref";
  ip->ceid = "xtf";
  
  return 0;
}

int
isp_glos_list(Isp *ip)
{
  /* ltab is malloc data */
  if (!isp_glos_letter_id(ip))
    return 1;

  if (ip->glosdata.let)
    {
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

  if (isp_glos_pmax(ip))
    return 1;

  if (ip->glosdata.xis)
    if (isp_glos_gxis(ip))
      return 1;
  
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
	  char *let = s;
	      
	  while (*l && '\t' != *l)
	    {
	      if ('\'' == *l)
		*l = (char)31;
	      ++l;
	    }
	  if (*l)
	    *l++ = '\0';
	      
	  while (*l && '\t' != *l)
	    ++l;

	  if (*l)
	    *l++ = '\0';
	  if (*let && !isspace(*let))
	    fprintf(lfp, "<p><a href=\"javascript://\" onclick=\"act_letter('%s')\"><span>%s</span></a></p>", let, let);

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
  if (ip->glosdata.let && !strcmp(ip->glosdata.let, "entry_ids"))
    {
      ip->glosdata.lbase = ip->glosdata.let;
    }
  else
    {
      int len = strlen(ip->glosdata.let);
      char *l = (char*)ip->glosdata.ltab;
      while (*l)
	{
	  if (!strncmp(l, ip->glosdata.let, len) && '\t' == l[len])
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
	  ip->err = PX_ERROR_START "isp_glos_glid: letter %s not found in letters.tab\n";
	  ip->errx = ip->glosdata.let;
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

int
isp_glos_item(Isp *ip)
{
  if (!ip->glosdata.web)
    {
      ip->glosdata.web = (ccp)pool_copy((ucp)ip->glosdata.dir, ip->p);
      char *pub = strstr((char *)ip->glosdata.web, "02pub");
      pub[02] = pub[03] = pub[04] = 'w';
    }
  ip->glosdata.ipath = (char*)pool_alloc(strlen(ip->glosdata.web)+strlen(ip->item)+strlen("/.html0"), ip->p);
  sprintf((char*)ip->glosdata.ipath, "%s/%s.html", ip->glosdata.web, ip->item);
  if (access(ip->glosdata.ipath, R_OK))
    {
      ip->err = PX_ERROR_START "isp_glos_item: %s not found\n";
      ip->errx = ip->glosdata.ipath;
      return 1;
    }
  else
    ip->itemdata.html = ip->glosdata.ipath;

  char *dbifn = "entry-indexes";
  Dbi_index *dp = dbx_init(ip->glosdata.dir, dbifn);
  if (dp)
    {
      char ikey[strlen(ip->glosdata.lbase)+strlen(ip->item)+2];
      sprintf(ikey, "%s:%s", ip->item, ip->glosdata.lbase);
      char *k = (char*)dbx_key(dp, ikey, NULL);
      if (!k)
	{
	  ip->err = PX_ERROR_START "key %s not found in item db\n";
	  ip->errx = (ccp)pool_copy((ucp)ikey, ip->p);
	  return 1;
	}
      ip->itemdata.index = (ccp)pool_copy((ucp)k, ip->p);
      int this = atoi(k);
      if (this > 1) {
	sprintf(ikey,"%s:%d",ip->glosdata.lbase, this-1);
	k = (char*)dbx_key(dp, ikey, NULL);
	if (k)
	  ip->itemdata.prev = (ccp)pool_copy((ucp)k, ip->p);
      }
      sprintf(ikey,"%s:%d",ip->glosdata.lbase, this+1);
      k = (char*)dbx_key(dp, ikey, NULL);
      if (k)
	ip->itemdata.next = (ccp)pool_copy((ucp)k, ip->p);
      dbx_term(dp);
    }
  else
    {
      char buf[strlen(ip->glosdata.dir)+strlen(dbifn)+6];
      sprintf(buf, "%s/%s.dbh", ip->glosdata.dir, dbifn);
      ip->errx = (ccp)pool_copy((ucp)buf, ip->p);
      ip->err = PX_ERROR_START "failed to open %s\n";
      return 1;
    }
  return 0;
}