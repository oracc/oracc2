#include <oraccsys.h>
#include "../pxdefs.h"
#include "iss.h"

const char **
text_array(Isp *ip, const char *tmpdir, const char **items, int imax, char **tmem, int *tmax)
{
  char **t = calloc(imax+1 , sizeof(char *));
  int tcount = 0, i, h_start = 0;

  const char *itemdp_dir = ip->tmp_dir ? ip->tmp_dir : ip->cache.sub;
  char *dbifn = "hilite";
  ip->itemdata.hilitedb = dbi_create(dbifn, itemdp_dir, 1024, 1, DBI_BALK);

#define ITEM_MAX_LEN	64 /* P123456.123456.1234.123 is 23 so 64 should be more than enough */
  char id[ITEM_MAX_LEN];

  for (i = 0; items[i]; ++i)
    {
      strncpy(id, items[i], ITEM_MAX_LEN);
      id[ITEM_MAX_LEN-1] = '\0'; /* insurance in case ID is overlong */
	
      char *dot = strchr(id, '.');
      if (dot)
	*dot = '\0';
      if (!i || strcmp(id, t[tcount-1]))
	{
	  t[tcount++] = (char *)pool_copy((ucp)id, ip->p);

	  if (i)
	    {
	      if (i - h_start == 1)
		dbi_add(ip->itemdata.hilitedb, (ucp)id, (void*)items[h_start],
			strlen(items[h_start])+1);
	      else
		{
		  char *h_str = vec_to_str((char**)(items + h_start), i - h_start, " ");
		  dbi_add(ip->itemdata.hilitedb, (ucp)id, h_str, strlen(h_str)+1);
		  free(h_str);
		}
	    }
	  h_start = i;
	}
    }
  if (i > h_start)
    {
      if (i - h_start == 1)
	dbi_add(ip->itemdata.hilitedb, (ucp)id, (void*)items[h_start],
		strlen(items[h_start])+1);
      else
	{
	  char *h_str = vec_to_str((char**)(items + h_start), i - h_start, " ");
	  dbi_add(ip->itemdata.hilitedb, (ucp)id, h_str, strlen(h_str)+1);
	  free(h_str);
	}
    }
  dbi_flush(ip->itemdata.hilitedb);

  if (tmax)
    *tmax = tcount;
  t = realloc(t, (1+tcount) * sizeof(char*));
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

