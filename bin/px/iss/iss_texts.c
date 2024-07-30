#include <oraccsys.h>
#include "../pxdefs.h"
#include "iss.h"
#include "../iso/iso.h"

static struct page *text_page(Isp *ip, struct page *p);

int
iss_texts(Isp *ip, struct page *p)
{
  int i = 0;
  while (i < p->used && ('#' == p->p[i][0] || '+' == p->p[i][0]))
    ++i;
  if (!strchr(p->p[i], '.'))
    return 0;
  struct page *tp = text_page(ip, p);

  /* save existing tsv/max/sort; add /t to sort and rebuild tsv/max */
  ip->cache.t_sort = (ccp)pool_alloc(strlen(ip->cache.sort)+3, ip->p);
  sprintf((char*)ip->cache.t_sort, "%s/t", ip->cache.sort);
  ip->cache.t_tsv = (ccp)pool_alloc(strlen(ip->cache.sort)+11, ip->p);
  sprintf((char*)ip->cache.t_tsv, "%s/t/pag.tsv", ip->cache.sort);
  ip->cache.t_max = (ccp)pool_alloc(strlen(ip->cache.sort)+11, ip->p);
  sprintf((char*)ip->cache.t_max, "%s/t/max.tsv", ip->cache.sort);

  struct stat sb;
  if (stat(ip->cache.t_sort, &sb) || !S_ISDIR(sb.st_mode))
    {
      if (ip->verbose)
	fprintf(stderr, "iss_data: creating %s\n", ip->cache.t_sort);
      if (mkdir(ip->cache.t_sort, 0775))
	{
	  ip->err = PX_ERROR_START "fatal: iss_data text-sort directory %s could not be created";
	  ip->errx = (ccp)pool_copy((ucp)ip->cache.t_sort, ip->p);
	}
    }

  return iss_data_sub(ip, tp, ip->cache.t_sort, ip->cache.t_tsv, ip->cache.t_max, ip->cache.t_mol);

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
  
  for (i = 0; i < p->used; ++i)
    {
      if ('#' == p->p[i][0])
	{
	  t[tcount++] = p->p[i];
	  last_t = -1;
	  continue;
	}
      strncpy(id, p->p[i], ITEM_MAX_LEN);
      id[ITEM_MAX_LEN-1] = '\0'; /* insurance in case ID is overlong */

      char *uscore = strchr(id, '_');
      if (uscore)
	{
	  *uscore = '\0'; /* strip translation info */
	}
      else
	{
	  char *dot = strchr(id, '.');
	  if (dot)
	    *dot = '\0';
	}
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

  t[tcount] = NULL;
  struct page *p2 = calloc(1, sizeof(struct page));
  p2->used = tcount;
  p2->texts = 1;
  t = realloc(t, (1+tcount) * sizeof(char*));
  p2->p = t;
  return p2;
}
