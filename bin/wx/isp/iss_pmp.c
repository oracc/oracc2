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

int
ztotal_get(int zcount, const char *s)
{
  while (*s)
    if (' ' == *s++)
      ++zcount;  
  return zcount;
}

void
ispmp_zooms(Isp *ip, unsigned char *f, int zmax)
{
  struct pgtell pt = { 0L, 0, 0L, 0 };
  int zoom = 0, zcount = 0, ztotal = 0;
  unsigned char *s = f;
  char zfn[strlen(ip->cache.sort)+strlen("-z012345679.pmp0")];
  while (*s)
    {
      if ('#' == *s)
	{
	  sprintf(zfn, "%s-z%d.pmp", ip->cache.sort, ++zoom);
	  FILE *zfp = fopen(zfn, "w");
	  zcount = 0;
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
		  ++zcount;
		  if (!(zcount%25))
		    {
		      if (!ztotal)
			ztotal = ztotal_get(zcount, (ccp)s);
		      pt.plen = (s - f) - pt.ptell;
		      fprintf(zfp, "%d/%d/%ld/%d/%ld/%d\n",
			      zmax, ztotal, pt.htell, pt.hlen, pt.ptell, pt.plen);
		      fclose(zfp);
#if 0
		      showbuf(f, pt.htell, pt.hlen, pt.ptell, pt.plen);
#endif
		      pt.ptell = (s - f) + 1;
		      pt.plen = 0;
		    }
		}
	      ++s;
	    }
	  if (zcount)
	    {
	      pt.plen = (s - f) - pt.ptell;
	      sprintf(zfn, "%s-z%d.pmp", ip->cache.sort, zoom);
	      FILE *zfp = fopen(zfn, "w");
	      fprintf(zfp, "%d/%d/%ld/%d/%ld/%d\n",
		      zmax, zcount, pt.htell, pt.hlen, pt.ptell, pt.plen);
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

void
ispmp_pages(Isp *ip, unsigned char *f, int imax)
{
  struct pgtell pt = { -1L, 0, 0L, 0 };
  long lasth_tell = 0L;
  int lasth_len = 0;
  int page = 1;
  int pcount = 0;
  unsigned char *s = f;
  char pfn[strlen(ip->cache.sort)+strlen(".pmp0")];
  sprintf(pfn, "%s.pmp", ip->cache.sort);
  FILE *pfp = fopen(pfn, "w");
  while (*s)
    {
      if ('#' == *s)
	{
	  if (pt.htell < 0)
	    {
	      pt.htell = s - f;
	      s = (ucp)strchr((ccp)s, ' ');
	      pt.hlen = (s - f) - pt.htell;
	      pt.ptell = (++s - f);
	    }
	  else
	    {
	      lasth_tell = s - f;
	      s = (ucp)strchr((ccp)s, ' ');
	      lasth_len = (s - f) - lasth_tell;
	    }
	}
      else
	{
	  if (' ' == *s || '\n' == *s)
	    {
	      ++pcount;
	      if (!(pcount%25))
		{
		  pt.plen = (s - f) - pt.ptell;
		  fprintf(pfp, "%d/%d/%ld/%d/%ld/%d\n",
			  page, imax, pt.htell, pt.hlen, pt.ptell, pt.plen);
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
	    }
	}
      ++s;
    }
  /* count from decremented s to remove terminal newline */
  pt.plen = (--s - f) - pt.ptell;
  if (pt.plen)
    {
      fprintf(pfp, "%d/%d/%ld/%d/%ld/%d\n",
	      page, imax, pt.htell, pt.hlen, pt.ptell, pt.plen);
#if 0      
      showbuf(f, pt.htell, pt.hlen, pt.ptell, pt.plen);
#endif
    }
  fclose(pfp);
}
#if 0
int
main(int argc, char **argv)
{
  unsigned char *f = loadfile((uccp)"/home/oracc/www/is.d/gudea/sux.r00002d/sort-123", NULL);
  ispmp_pages(f);
}
#endif
