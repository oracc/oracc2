#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"

static const char *etm_db_one_off(Isp *ip, const char *key);

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
  if (ip->glosdata.let)
    {
      const char *max = etm_db_one_off(ip, ip->glosdata.let);
      if (!strcmp(ip->glosdata.let, "entry_ids"))
	ip->glosdata.emax = max;
      else
	ip->glosdata.lmax = max;
    }
  return ip->err ? 1 : 0;
}

/* Reset cache.sub to be the glossary cache + the gxis name

   Then set lloc so it reflects the gxis instead of the glossary entries.
 */
int
isp_glos_gxis(Isp *ip)
{
  if (ip->force < 2)
    {
#if 1
      /* cache.sub already has gxis and has been xmkdir'd */
      ip->cache.use = ip->cache.sub;
#else
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
      ip->cache.use = ip->cache.out = ip->cache.sub = xcache;
#endif
    }

  ip->list_name = ip->glosdata.xis;

  if (isp_list_method(ip))
    return 1;

  ip->list_name = "list"; /* reset this so it uses the standard name for the xis extract */
  
  ip->lloc.path = (ccp)pool_alloc(strlen(ip->cache.sub)+6, ip->p);
  sprintf((char*)ip->lloc.path, "%s/list", ip->cache.sub);
  if (!ip->zoom || !isdigit(*ip->zoom))
    ip->zoom = "0";
  ip->cemd = "line";
  ip->show = "rref";
  ip->ceid = "xtf";
  
  return 0;
}

int
isp_glos_list(Isp *ip)
{
  if (!ip->glosdata.let && ip->zoom && !isdigit(*ip->zoom))
    ip->glosdata.let = ip->zoom;
  
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
       * We should only need to create ip->cache.zout once, but right now we recreate it every time;
       * if we go to a single create we will need to reread each time to add zoomed class
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
	    {
	      const char *zoomed = "";
	      if (ip->glosdata.let && !strcmp(let, ip->glosdata.let))
		zoomed = "class=\"zoomed\" ";
	      fprintf(lfp, "<p><a href=\"javascript://\" %sonclick=\"act_letter('%s')\"><span>%s</span></a></p>", zoomed, let, let);
	    }

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

const char *
isp_glos_etm(Isp *ip)
{
  return etm_db_one_off(ip, ip->item);
}

static const char *
etm_db_one_off(Isp *ip, const char *key)
{
  Dbi_index *dp = dbx_init(ip->glosdata.dir, "etm");
  const void *ret = NULL;
  if (dp)
    {
      if (!(ret = dbx_key(dp, key, NULL)))
	ip->err = px_err("key %s not in dbi %s/%s", key, pool_copy((ucp)ip->glosdata.dir, ip->p), "etm");
      /* ret is now memory malloc'ed in dbi lib */
      ret = dbi_detach_data(dp, NULL);
      dbx_term(dp);
    }
  else
    ip->err = px_err("unable to open dbi %s/%s", pool_copy((uccp)ip->glosdata.dir,ip->p), "etm");
  return ret;
}

int
isp_glos_entry(Isp *ip)
{
  List *args = list_create(LIST_SINGLE);
  list_add(args, (void*)ip->oracc);
  list_add(args, (void*)"/bin/ispent.sh");
  list_add(args, " ");
  list_add(args, (void*)ip->glosdata.ipath);
  unsigned char *syscmd = list_concat(args);

  if (ip->verbose)
    fprintf(stderr, "isp: isp_glos_entry: %s\n", syscmd);
  
  int sys;
  if ((sys = system((ccp)syscmd)))
    {
      int xstatus = WEXITSTATUS(sys);
      /*int ifexited = WIFEXITED(sys);*/
      if (xstatus > 1)
        {
          ip->itemdata.not = xstatus;
        }
      else
        {
          ip->err = PX_ERROR_START "isp_glos_entry failed system call:\n\t%s\n";
          ip->errx = (ccp)syscmd;
          return 1;
        }
    }
  
  return 0;
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
  if (ip->item && !ip->glosdata.ent)
    ip->glosdata.ent = ip->item;
  else if (ip->glosdata.ent)
    ip->item = ip->glosdata.ent;
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

  return 0;
}
