#include <oraccsys.h>
#include "../pxdefs.h"
#include "iss.h"
#include "../iso/iso.h"

static struct page *text_page(Isp *ip, struct page *p);

int
iss_texts(Isp *ip, struct page *p)
{
  int i;
  while (p->p[i] && ('#' == p->p[i][0] || '+' == p->p[i][0]))
    ++i;
  if (!strchr(p->p[i], '.'))
    return 0;
  struct page *tp = text_page(ip, p);

  /* save existing tsv/max/sort; add /t to sort and rebuild tsv/max */
  ip->cache.t_sort = (ccp)pool_alloc(strlen(ip->cache.sort)+3, ip->p);
  sprintf((char*)ip->cache.t_sort, "%s/t", ip->cache.sort);
  ip->cache.t_tsv = (ccp)pool_alloc(strlen(ip->cache.tsv)+3, ip->p);
  sprintf((char*)ip->cache.t_tsv, "%s/t", ip->cache.tsv);
  ip->cache.t_max = (ccp)pool_alloc(strlen(ip->cache.max)+3, ip->p);
  sprintf((char*)ip->cache.t_max, "%s/t", ip->cache.max);
  
  return iss_data_sub(ip, tp, ip->cache.t_sort, ip->cache.t_tsv, ip->cache.t_max);

  /* restore previous tsv/max/sort */
}

static struct page *
text_page(Isp *ip, struct page *p)
{
  char **t = calloc(p->used+1 , sizeof(char *));
  int tcount = 0, i, h_start = 0;
  char **items = p->p;

  const char *itemdp_dir = ip->tmp_dir ? ip->tmp_dir : ip->cache.sub;
  char *dbifn = "hilite";
  ip->itemdata.hilitedb = dbi_create(dbifn, itemdp_dir, 1024, 1, DBI_BALK);

#define ITEM_MAX_LEN	64 /* P123456.123456.1234.123 is 23 so 64 should be more than enough */
  char id[ITEM_MAX_LEN];

  int last_t = -1;
  
  for (i = 0; p->p[i]; ++i)
    {
      if ('#' == p->p[i][0])
	{
	  t[tcount++] = p->p[i];
	  last_t = -1;
	  continue;
	}
      strncpy(id, p->p[i], ITEM_MAX_LEN);
      id[ITEM_MAX_LEN-1] = '\0'; /* insurance in case ID is overlong */
	
      char *dot = strchr(id, '.');
      if (dot)
	*dot = '\0';
      if (last_t < 0 || strcmp(id, t[last_t]))
	{
	  last_t = tcount;

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

#if 0
  if (tmax)
    *tmax = tcount;
#endif

  t[tcount] = NULL;
  struct page *p2 = calloc(1, sizeof(struct page));
  p2->used = tcount;
  t = realloc(t, (1+tcount) * sizeof(char*));
  p2->p = t;
  return p2;
}
