#include <oraccsys.h>
#include "isp.h"

static void
file_copy_chunk(FILE *from, unsigned long start, int len, FILE *to)
{
  fseek(from,start, SEEK_SET);
  char *buf = malloc(len+1);
  fread(buf, 1, len, from);
  char *s = buf;
  while (*s)
    if (' ' == *s)
      *s++ = '\n';
    else
      ++s;
  fwrite(buf, 1, len, to);
  free(buf);
}

static void
md_parse(char *mline, struct isp_mapdata *mdp)
{
  char *next;
  mdp->zmax = strtoul(mline, &next, 10);
  mdp->zimx = strtoul(next+1, &next, 10);
  mdp->htell = strtoul(next+1, &next, 10);
  mdp->hlen = (int)strtoul(next+1, &next, 10);
  mdp->ptell = strtoul(next+1, &next, 10);
  mdp->plen = (int)strtoul(next+1, &next, 10);
}

static int
compute_ranges(Isp *ip)
{
    char mbuf[strlen(ip->cache.sort)+strlen(ip->zoom)+strlen(".pmp0")];
  if (atoi(ip->zoom))
    sprintf(mbuf, "%s-z%s.pmp", ip->cache.sort, ip->zoom);
  else
    sprintf(mbuf, "%s.pmp", ip->cache.sort);

  /* compute the start line in the .pmp */
  int multiplier = 1;
  if (!strcmp(ip->psiz, "50"))
    multiplier = 2;
  else if (!strcmp(ip->psiz,"100"))
    multiplier = 4;

  int pg = atoi(ip->page) * multiplier;  
  char *mline = nth_line(mbuf, pg, multiplier>1);
  char *mmline = NULL;
  md_parse(mline, &ip->md1);

  if (multiplier > 1)
    {
      mmline = nth_line(NULL, pg+multiplier, 0);
      if (mmline)
	md_parse(mmline, &ip->md2);
      else
	{
	  ip->err = "page out of range";
	  return 1;
	}
    }
  else
    ip->md2.plen = -1;
  return 0;
}

static int
create_page_input(Isp *ip)
{
  char pagbuf[strlen(ip->cache.page)+1];
  strcpy(pagbuf, ip->cache.page);
  char *pd = strrchr(pagbuf, '.');
  strcpy(pd+1, "pag");
  ip->cache.pgin = (ccp)pool_copy((uccp)pagbuf, ip->p);
  FILE *zfp = fopen(pagbuf, "w");
  if (zfp)
    {
      if (ip->verbose)
	fprintf(stderr, "isp: isp_cache_page: writing %s\n", pagbuf);
      FILE *sfp = fopen(ip->cache.sort,"r");
      if (sfp)
	{
	  if (ip->md1.htell < ip->md1.ptell)
	    file_copy_chunk(sfp, ip->md1.htell, ip->md1.hlen, zfp);
	  if (ip->md2.plen >= 0)
	    file_copy_chunk(sfp, ip->md1.ptell, ip->md2.ptell+ip->md2.plen, zfp);
	  else
	    file_copy_chunk(sfp, ip->md1.ptell, ip->md1.plen, zfp);
	  fclose(sfp);
	  fclose(zfp);
	}
      else
	{
	  fclose(zfp);
	  ip->err = "unable to read page file when building";
	}
    }
  else
    ip->err = "unable to write to zoom-page-file";
  return ip->err ? 1 : 0;
}

int
create_page_div(Isp *ip)
{
  List *args = list_create(LIST_SINGLE);
  list_add(args, (void*)ip->oracc);
  list_add(args, (void*)"/bin/ispdiv.sh");
  list_add(args, " ");
  list_add(args, (void*)ip->project);
  list_add(args, " ");
  list_add(args, (void*)ip->cache.pgin);
  list_add(args, " ");
  list_add(args, (void*)ip->cache.page);
  unsigned char *syscmd = list_concat(args);
  if (ip->verbose)
    fprintf(stderr, "isp: create_page_div: %s\n", syscmd);
  system((ccp)syscmd);
  return 0;
}

int
isp_cache_page(Isp *ip)
{
  char pbuf[strlen(ip->cache.sub)+strlen(ip->zoom)+strlen(ip->page)+strlen("-123-z-p.div0")];
  sprintf(pbuf, "%s/%s-z%s-p%s.div", ip->cache.sub, ip->perm, ip->zoom, ip->page);
  ip->cache.page = (ccp)pool_copy((uccp)pbuf, ip->p);

  char zbuf[strlen(ip->cache.sub)+strlen(ip->zoom)+strlen("-123-z.otl0")];
  sprintf(zbuf, "%s/%s-z%s.otl", ip->cache.sub, ip->perm, ip->zoom);
  ip->cache.zout = (ccp)pool_copy((uccp)zbuf, ip->p);

  if (ip->verbose)
    fprintf(stderr, "isp: isp_cache_page: need %s:::%s\n", zbuf, pbuf);

  if (!ip->force)
    {
      int need_zout = 0;
      if (!access(ip->cache.zout, F_OK))
	{
	  if (access(ip->cache.zout, R_OK))
	    {
	      ip->err = ISP_ERROR_START "cache zoom %s exists but is unreadable\n";
	      ip->errx = ip->cache.zout;
	      return 1;
	    }
	}

      if (need_zout)
	if (ispo_zoutline(ip))
	  return 1;
	
      if (!access(ip->cache.page, F_OK))
	{
	  if (!access(ip->cache.page, R_OK))
	    return 0;
	  else
	    {
	      ip->err = ISP_ERROR_START "cache page %s exists but is unreadable\n";
	      ip->errx = ip->cache.page;
	      return 1;
	    }
	}
    }

  if (ispo_zoutline(ip))
    return 1;

  if (compute_ranges(ip))
    return 1;

  if (create_page_input(ip))
    return 1;

  
  return create_page_div(ip);
}
