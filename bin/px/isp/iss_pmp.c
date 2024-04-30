#include <oraccsys.h>
#include "isp.h"

struct pgtell
{
  
  long htell;
  int hlen;
  long ptell;
  int plen;
};

#if 0
char *
chunk(unsigned char *s, long from, int len)
{
  char buf[len+1];
  strncpy(buf, (char*)s+from, len);
  buf[len] = '\0';
  return strdup(buf);
}

void
showbuf(unsigned char *s, long h, int hlen, long p, int plen)
{
  printf("h %ld/%d='%s'\n", h, hlen, chunk(s, h, hlen));
  printf("p %ld/%d='%s'\n", p, plen, chunk(s, p, plen));
  fflush(stdout);
}
#endif

static void
md_dump(FILE *fp, int total, long htell, int hlen, long ptell, int plen)
{
  fprintf(fp, "%d/%ld/%d/%ld/%d\n", total, htell, hlen, ptell, plen);
}

static int
ztotal_get(int zcount, const char *s)
{
  while (*s)
    {
      if (' ' == *s)
	++zcount;
      else if ('\n' == *s)
	break;
      ++s;
    }
  return zcount;
}

void
ispmp_zooms(Isp *ip, unsigned char *f, int zmax)
{
  struct pgtell pt = { 0L, 0, 0L, 0 };
  int zoom = 0, zcount = 0, zpcount = 0, ztotal = 0;
  unsigned char *s = f;
  char zfn[strlen(ip->cache.sort)+strlen("-z012345679.pmp0")];
  while (*s)
    {
      if ('#' == *s)
	{
	  sprintf(zfn, "%s-z%d.pmp", ip->cache.sort, ++zoom);
	  FILE *zfp = fopen(zfn, "w");
	  zcount = zpcount = 0;
	  ztotal = 0;
	  pt.htell = s - f;
	  s = (ucp)strchr((ccp)s, ' ');
	  pt.hlen = (s - f) - pt.htell;
	  /* we want the tell location after the space but we don't
	     want to skip the space because we count items by their
	     preceding space characters */
	  pt.ptell = (s - f) + 1;
	  while ('\n' != *s)
	    {
	      if (' ' == *s)
		{
		  while (s[1] && !isspace(s[1]))
		    ++s;
		  ++zcount;
		  ++zpcount;
		  if (zpcount==25)
		    {
		      if (!ztotal)
			ztotal = ztotal_get(zcount, (ccp)s);
		      unsigned char *l = s+1;
		      pt.plen = (l - f) - pt.ptell;
		      md_dump(zfp, ztotal, pt.htell, pt.hlen, pt.ptell, pt.plen);
#if 0
		      showbuf(f, pt.htell, pt.hlen, pt.ptell, pt.plen);
#endif
		      pt.ptell = (s - f) + 2;
		      pt.plen = 0;
		      zpcount = 0;
		    }
		}
	      ++s;
	    }
	  if (zpcount)
	    {
	      pt.plen = (s - f) - pt.ptell;
	      md_dump(zfp, zcount, pt.htell, pt.hlen, pt.ptell, pt.plen);
#if 0
	      showbuf(f, pt.htell, pt.hlen, pt.ptell, pt.plen);
#endif
	    }
	  fclose(zfp);
	  memset(&pt, '\0', sizeof(struct pgtell));
	  ++s;
	}
    }
}

static void
dumpitem(Isp *ip, const char **items, int itemnth, int page, int zoomth, int zoomp)
{
  const char *prev = NULL;
  const char *next = NULL;

  if (itemnth > 0)
    {
      prev = strchr(items[itemnth-1],':');
      if (prev)
	++prev;
      else
	prev = items[itemnth-1];
    }
  else
    prev = "#";
  if (items[itemnth+1])
    {
      next = strchr(items[itemnth+1],':');
      if (next)
	++next;
      else
	next = items[itemnth+1];
    }
  else
    next = "#";
  char key[8];
  char *s = strchr(items[itemnth], ':');
  if (s)
    strcpy(key, s+1);
  else
    strcpy(key, items[itemnth]);
	
  int n = snprintf(NULL, 0, "%d\t%d\t%d\t%s\t%s", page, zoomth, zoomp, prev, next);
  char data[n+1];
  sprintf(data, "%d\t%d\t%d\t%s\t%s", page, zoomth, zoomp, prev, next);
  dbi_add(ip->itemdata.dp, (ucp)key, data, strlen(data)+1);
}

const char **
item_array(char *s, int imax, char **mem)
{
  char *is = strdup(s);
  char **ip = malloc((imax+1)*sizeof(char *));
  int i = 0;
  *mem = is;
  s = is;
  while (*s)
    {
      if ('#' == *s)
	s = strchr(s, ' ');
      else if (!isspace(*s))
	{
	  ip[i++] = s;
	  while (*s && !isspace(*s))
	    ++s;
	  if (*s)
	    *s++ = '\0';
	}
      else if (isspace(*s))
	++s;
    }
  ip[i] = NULL;
  return (const char **)ip;
}

void
ispmp_pages(Isp *ip, unsigned char *f, int imax)
{
  struct pgtell pt = { -1L, 0, 0L, 0 };
  long lasth_tell = 0L;
  int lasth_len = 0;
  int page = 1;
  int pcount = 0;
  int zoomth = 0, zoomp = 1;
  unsigned char *s = f;

  char dbifn[strlen(ip->perm)+strlen("-itm0")];
  sprintf(dbifn, "%s-itm", ip->perm);
  ip->itemdata.dp = dbi_create(dbifn, ip->cache.sub, 1024, 1, DBI_BALK);

  char pfn[strlen(ip->cache.sort)+strlen(".pmp0")];
  sprintf(pfn, "%s.pmp", ip->cache.sort);
  FILE *pfp = fopen(pfn, "w");

  char *imem = NULL;
  const char **items = item_array((char*)s, imax, &imem);
  int itemnth = 0;
  
  while (*s)
    {
      if ('#' == *s)
	{
	  ++zoomth;
	  zoomp = 1;
	  if (pt.htell < 0)
	    {
	      pt.htell = s - f;
	      s = (ucp)strchr((ccp)s, ' ');
	      pt.hlen = (s - f) - pt.htell;
	      pt.ptell = ((s+1) - f);
	    }
	  else
	    {
	      lasth_tell = s - f;
	      s = (ucp)strchr((ccp)s, ' ');
	      lasth_len = (s - f) - lasth_tell;
	    }
	  --s;
	}
      else
	{
	  if (' ' == *s || '\n' == *s)
	    {
	      ++pcount;
	      if (!(pcount%25))
		{
		  ++zoomp;
		  pt.plen = (s - f) - pt.ptell;
		  md_dump(pfp, imax, pt.htell, pt.hlen, pt.ptell, pt.plen);
#if 0
		  showbuf(f, pt.htell, pt.hlen, pt.ptell, pt.plen);
#endif
		  pt.ptell = (s - f) + 1;
		  pt.plen = 0;
		  if (lasth_tell)
		    {
		      pt.htell = lasth_tell;
		      pt.hlen = lasth_len;
		    }
		  ++page;
		}
	      if (s[1] && '#' != s[1] && !isspace(s[1]))
		{
		  dumpitem(ip, items, itemnth, page, zoomth, zoomp);
		  ++itemnth;
		}
	    }
	}
      ++s;
    }
  /* count from decremented s to remove terminal newline */
  pt.plen = (--s - f) - pt.ptell;
  if (pt.plen)
    {
      md_dump(pfp, imax, pt.htell, pt.hlen, pt.ptell, pt.plen);
#if 0      
      showbuf(f, pt.htell, pt.hlen, pt.ptell, pt.plen);
#endif
    }
  fclose(pfp);
  dbi_flush(ip->itemdata.dp);
  free(imem);
  free(items);
}
#if 0
int
main(int argc, char **argv)
{
  unsigned char *f = loadfile((uccp)"/home/oracc/www/is.d/gudea/sux.r00002d/sort-123", NULL);
  ispmp_pages(f);
}
#endif
