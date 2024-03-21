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

void
ispmp_zooms(unsigned char *f)
{
  struct pgtell pt = { 0L, 0, 0L, 0 };
  int zoom = 0;
  int zcount = 0;
  unsigned char *s = f;
  while (*s)
    {
      if ('#' == *s)
	{
	  ++zoom;
	  zcount = 0;
	  pt.htell = s - f;
	  s = (ucp)strchr((ccp)s, ' ');
	  pt.hlen = (s - f) - pt.htell;
	  pt.ptell = (++s - f);
	  while ('\n' != *s)
	    {
	      if (' ' == *s)
		{
		  ++zcount;
		  if (!(zcount%25))
		    {
		      pt.plen = (s - f) - pt.ptell;
		      fprintf(stderr, "z%d\t%ld/%d/%ld/%d\n",
			      zoom, pt.htell, pt.hlen, pt.ptell, pt.plen);
#if 0
		      showbuf(f, pt.htell, pt.hlen, pt.ptell, pt.plen);
#endif
		      pt.ptell = (s - f) + 1;
		      pt.plen = 0;
		    }
		}
	      ++s;
	    }
	  if (pt.ptell)
	    {
	      pt.plen = (s - f) - pt.ptell;
	      fprintf(stderr, "z%d\t%ld/%d/%ld/%d\n",
		      zoom, pt.htell, pt.hlen, pt.ptell, pt.plen);
#if 0
	      showbuf(f, pt.htell, pt.hlen, pt.ptell, pt.plen);
#endif
	      memset(&pt, '\0', sizeof(struct pgtell));
	    }
	  ++s;
	}
    }
}

void
ispmp_pages(unsigned char *f)
{
  struct pgtell pt = { -1L, 0, 0L, 0 };
  long lasth_tell = 0L;
  int lasth_len = 0;
  int page = 1;
  int pcount = 0;
  unsigned char *s = f;
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
		  fprintf(stderr, "p%d\t%ld/%d/%ld/%d\n",
			  page, pt.htell, pt.hlen, pt.ptell, pt.plen);
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
      fprintf(stderr, "p%d\t%ld/%d/%ld/%d\n",
	      page, pt.htell, pt.hlen, pt.ptell, pt.plen);
#if 0      
      showbuf(f, pt.htell, pt.hlen, pt.ptell, pt.plen);
#endif
    }
}
#if 0
int
main(int argc, char **argv)
{
  unsigned char *f = loadfile((uccp)"/home/oracc/www/is.d/gudea/sux.r00002d/sort-123", NULL);
  ispmp_pages(f);
}
#endif
