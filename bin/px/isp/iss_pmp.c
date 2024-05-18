#include <oraccsys.h>
#include "../px.h"
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
dumpitem(Isp *ip, const char **items, int itemnth, int page, int zoom, int zpage, int zindex)
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
  /* It shouldn't be necessary to test items[itemnth] here */
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
  char key[strlen(items[itemnth])+1];
  char *s = strchr(items[itemnth], ':');
  if (s)
    strcpy(key, s+1);
  else
    strcpy(key, items[itemnth]);
	
  int n = snprintf(NULL, 0, "%d\t%d\t%d\t%d\t%d\t%s\t%s", itemnth, page, zoom, zpage, zindex, prev, next);
  char data[n+1];
  sprintf(data, "%d\t%d\t%d\t%d\t%d\t%s\t%s", itemnth, page, zoom, zpage, zindex, prev, next);
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

const char **
text_array(Isp *ip, const char *tmpdir, const char **items, int imax, char **tmem, int *tmax)
{
  char **t = malloc(imax * sizeof(char *));
  int tcount = 0, i, h_start = 0;

#if 1
  const char *itemdp_dir = ip->tmp_dir ? ip->tmp_dir : ip->cache.sub;
  char *dbifn = "hilite";
  ip->itemdata.hilitedb = dbi_create(dbifn, itemdp_dir, 1024, 1, DBI_BALK);
#else
  ip->cache.hilite = (ccp)pool_alloc(strlen(tmpdir)+strlen("/hilite.tab0"), ip->p);
  sprintf((char*)ip->cache.hilite, "%s/%s", tmpdir, "hilite.tab");
  FILE *hilitefp = fopen(ip->cache.hilite, "w");
#endif

  for (i = 0; items[i]; ++i)
    {
      char id[strlen(items[i])];
      strcpy(id, items[i]);
      char *dot = strchr(id, '.');
      if (dot)
	*dot = '\0';
      if (!i || strcmp(id, t[tcount-1]))
	{
	  t[tcount++] = (char *)pool_copy((ucp)id, ip->p);
#if 1
	  if (i - h_start == 1)
	    dbi_add(ip->itemdata.hilitedb, (ucp)id, (void*)items[h_start],
		    strlen(items[h_start])+1);
	  else
	    {
	      char *h_str = vec_to_str((char**)(items + h_start), i - h_start, " ");
	      dbi_add(ip->itemdata.hilitedb, (ucp)id, h_str, strlen(h_str)+1);
	      free(h_str);
	    }
	  h_start = i;
#else
	  if (i)
	    fputc('\n', hilitefp);
	  fprintf(hilitefp, "%s\t%s", id, items[i]);
#endif
	}
      else
	{
#if 0
	  fprintf(hilitefp, " %s", items[i]);
#endif
	}
    }
#if 1
  dbi_flush(ip->itemdata.hilitedb);
#else
  fclose(hilitefp);
#endif

  if (tmax)
    *tmax = tcount;
  t = realloc(t, tcount * sizeof(char*));
  return (const char **)t;
}

static int
textcmp(const char *t1, const char *t2)
{
  const char *c1 = strchr(t1, ':');
  const char *c2 = strchr(t2, ':');
  int len = 7;
  if (c1 && c2)
    {
      if ((c1-t1) != (c2-t2))
	return 1;
      else
	{
	  const char *d1 = strchr(c1, '.');
	  len = d1 - t1;
	  return strncmp(t1, t2, len);
	}
    }
  else
    {
      if (!c1 && !c2)
	return strncmp(t1, t2, len);
      else
	return 1;
    }
  return 0;
}

/* Three values are tracked for zoom data:
 *
 * izoom/tzoom = item/text zoom number
 * izpage/tzpage = item/text zoom page
 * izcount/tzcount = item/text zoom index, i.e., index into
 * 	total number of entries in zoomed section
 *
 */

int
ispmp_pages(Isp *ip, unsigned char *f, int imax)
{
  struct pgtell pt = { -1L, 0, 0L, 0 };
  long lasth_tell = 0L;
  int lasth_len = 0;
  int page = 1;
  int pcount = 0;
  int izoom = 0, izcount = 0; /* izpage is computed at item dump */
  int tzoom = 0, tzcount = 0; /* tzpage is computed at text dump */
  unsigned char *s = f;
  const char *itemdp_dir = ip->tmp_dir ? ip->tmp_dir : ip->cache.sub;

  char dbifn[strlen(ip->perm)+strlen("-itm0")];
  sprintf(dbifn, "%s-itm", ip->perm);
  ip->itemdata.dp = dbi_create(dbifn, itemdp_dir, 1024, 1, DBI_BALK);

  if (!ip->itemdata.dp)
    {
      ip->err = (ccp)px_err("unable to create item data dbi %s/%s\n", itemdp_dir, dbifn);
      return 1;
    }
  
  char pfn[strlen(ip->cache.sort)+strlen(".pmp0")];
  sprintf(pfn, "%s.pmp", ip->cache.sort);
  FILE *pfp = fopen(pfn, "w");

  char *imem = NULL, *tmem = NULL;
  const char **items = item_array((char*)s, imax, &imem);
  const char **texts = NULL;
  int itemnth = 0;
  int textth = 0;

  if (strchr(items[0], '.'))
    {
      int tmax;
      texts = text_array(ip, itemdp_dir, items, imax, &tmem, &tmax);
      char tm[32]; sprintf(tm, "%d", tmax);
      dbi_add(ip->itemdata.dp, (ucp)"tmax", tm, strlen(tm)+1);
    }
  
  while (*s)
    {
      if ('#' == *s)
	{
	  izcount = tzcount = 0; /* count of items in this zoom unit so far */
	  ++izoom;
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
		  if (texts)
		    {
		      if (itemnth && textcmp(items[itemnth-1], items[itemnth]))
			{
			  /* Only dump information when it's a new text */
			  int tpage = (textth / 25) + ((textth % 25) ? 1 : 0);
			  int tzpage = (tzcount / 25) + ((tzcount % 25) ? 1 : 0);
			  dumpitem(ip, texts, textth, tpage, tzoom, tzpage, tzcount);
			  ++textth;
			  ++tzcount;
			}
		    }
		  else
		    {
		      int izpage = (izcount / 25) + ((izcount % 25) ? 1 : 0);
		      dumpitem(ip, items, itemnth, page, izoom, izpage, izcount);
		    }
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
  return 0;
}

