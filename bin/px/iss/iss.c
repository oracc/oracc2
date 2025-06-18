#include <oraccsys.h>
#include "../pxdefs.h"
#include "iss.h"

FILE *fpag = NULL;
Hash *seen = NULL;
Pool *pg2_pool;
struct item *items = NULL;
int items_used = 0;
struct sortinfo *sip;

extern int nsortfields;
int nheadfields = 0;
int *headfields = NULL;
const char *terminal_sort_field = "designation";
char *tmp;

struct sortinfo *sip;

#if 0
extern int use_linkmap;
#endif

#include "iss_sk_lookup.c"

static char *
append_designation(Isp *ip, const char *s)
{
#if 1
  char buf[strlen(s)+strlen(",designation0")];
  strcpy(buf, s);
  strcat(buf, ",designation");
  s = (ccp)pool_copy((uccp)buf,ip->p);
#else
  /* need do figure out what the sort_final stuff in p2 was; for now we do a dumb append */
  char *tmp2 = malloc(strlen(entry)+strlen(p2opts->sort_final)+2);
  sprintf(tmp2, "%s,%s", entry, p2opts->sort_final);
  s = (char*)pool_copy((unsigned char *)tmp2,ip->p);
  free(tmp2);
#endif
  return (char*)s;
}

static unsigned char *
adjust_s(unsigned char *stop, unsigned char *s)
{
  unsigned char *t = s;
  while (t > stop && isspace(t[-1]))
    --t;
  if (isspace(*t))
    *t = '\0';
  while (isspace(s[1]))
    ++s;
  return s;
}

static int
is_lang_id(const char *s)
{
  while (isalnum(*s) || '-' == *s)
    ++s;
  if (*s == '.')
    ++s;
  return 'x' == *s;
}

struct item *
pg_load(Isp *ip, int *nitems)
{
  char *buf = NULL, *s;
  size_t buflen;
  if (!(buf = (char*)loadfile((unsigned const char *)ip->cache.list,&buflen)))
    {
      ip->err = px_err("failed to load list file %s", ip->cache.list);
      return NULL;
    }
  s = buf;
#if 1
  {
    register size_t nitems = 0;
    register char *b = buf;
    while (*b)
      if ('\n' == *b++)
	++nitems;
    items = malloc((nitems + 2) * sizeof(struct item)); /* alloc +2 in case no \n at EOF */
  }
#else
  /* we know that each entry takes up at least 8 bytes [2025-06-18: not true since ood support] */
  items = malloc(((buflen / 8) + 1) * sizeof(struct item));
#endif

  while (s - buf < buflen)
    {
      /* set up orig_s so it points to the PQX, which may be
	 qualified, and so the end of the (Q)QPX is null-terminated;
	 colon points to the QPX character whether the QPX is
	 qualified or not */
      char *colon = NULL, *dot;
      char *orig_s = s;
      colon = strchr((char*)s,':');
      if (colon)
	s = ++colon;
      if (ip->ood || *s == 'P' || *s == 'Q' || *s == 'X' || is_lang_id((char *)s) || *s == 'o')
	{
	  items[items_used].s = (ucp)orig_s;
	  while (*s && '\n' != *s)
	    ++s;
	  s = (char*)adjust_s((ucp)buf,(ucp)s);
	  *s++ = '\0';
	  items[items_used].qpq = pool_copy((ucp)orig_s, ip->p);
	  if (colon)
	    {
	      int offset = colon - orig_s;
	      items[items_used].pq = items[items_used].qpq + offset;
	    }
	  else
	    items[items_used].pq = items[items_used].qpq;
		
	  if ((dot = strchr((const char *)items[items_used].pq,'.')))
	    {
	      *dot++ = '\0';
	      items[items_used].lkey = atoi(dot);
	    }
	  else
	    items[items_used].lkey = 0;
	  ++items_used;
	}
      else
	{
	  ip->err = px_err("syntax error line %d in list file %s; expected QPX-number",
			   items_used,
			   ip->cache.list);
	  return NULL;
	}
    }
  *nitems = items_used;
  return items;
}

static int *
set_keys(const char *s, int *nfields)
{
  int nkeys = 1, *fields;
  const char *t;
  
  if (!s || !strcmp(s,"none"))
    {
      *nfields = 0;
      return NULL;
    }

  t = s;
  while (*t)
    if (*t++ == ',')
      ++nkeys;
  fields = malloc(sizeof(int)*(nkeys+1));
  for(t = s, nkeys = 0; *t; ++nkeys)
    {
      if (isdigit(*t))
	fields[nkeys] = atoi(t);
      else
	fields[nkeys] = sk_lookup(t);
      while (*t && ',' != *t)
	++t;
      if (',' == *t)
	++t;
    }
  *nfields = nkeys;
  fields[nkeys] = -1;
  return fields;
}

const char *
iss_perm(Isp *ip, const char *sort_keys, int nkeys)
{
  if (nkeys < 2 || (nkeys == 2 && !strcmp(ip->perm, "12")) || (nkeys == 3  && !strcmp(ip->perm, "123")))
    return sort_keys;

  const char *k[nkeys];
  char *s = (char*)pool_copy((ucp)sort_keys, ip->p);
  k[0] = s;
  while (*s && ',' != *s)
    ++s;
  if (',')
    {
      *s++ = '\0';
      k[1] = s;
      while (*s && ',' != *s)
	++s;
      if (',')
	{
	  *s++ = '\0';
	  k[2] = s;
	}
    }
  const char *p[nkeys];
  if (nkeys == 2)
    {
      p[0] = k[1];
      p[1] = k[0];
      char *ret = malloc(strlen(sort_keys)+2);
      sprintf(ret, "%s,%s", k[1], k[0]);
      return ret;
    }
  else
    {
      int i;
      const char *perm = ip->perm;
      for (i = 0; perm[i]; ++i)
	p[i] = k[perm[i]-'1'];
      char *ret = malloc(strlen(sort_keys)+3);
      sprintf(ret, "%s,%s,%s", p[0],  p[1], p[2]);
      return ret;
    }
  return NULL;
}

int
iss_sort(Isp *ip)
{
  struct item *items = NULL, **pitems = NULL;
  struct page *pages = NULL;
  int nitems = 0;

  const char *perm_keys = iss_perm(ip, ip->curr_cfg->sort_fields, ip->zlev);
  
  seen = hash_create(1024);
  pg2_pool = pool_init();

  if (NULL == (sip = si_load_csi(ip)))
    {
      ip->err = px_err("failed to load csi file %s", ip->cache.csi);
      return 1;
    }

  u4 i;
  Hash *known_fields = hash_create(5);
  for (i = 0; i < sip->nmapentries; ++i)
    hash_add(known_fields, sip->fields[i].n, "");
  char *s = strdup(perm_keys), *f, *tok;
  int badf = 0;
  tok = s;
  int designation_ok = 0;
  while ((f = strtok(tok, ",")))
    {
      tok = NULL;
      if (!hash_find(known_fields, (uccp)f))
	{
	  fprintf(stderr, "ispsortx: unknown field %s\n", f);
	  ++badf;
	}
      else if (!strcmp(f, "designation"))
	designation_ok = 1;
    }
  hash_free(known_fields, NULL);
  free(s);

  if (!designation_ok)
    perm_keys = append_designation(ip, perm_keys);

  if (badf)
    {
      ip->err = "bad fields in sort specification";
      return 1;
    }
  
  const char *heading_keys = (ccp)pool_copy((uccp)perm_keys, pg2_pool);
  char *des = strstr(heading_keys, "designation");
  if (des && !strchr(des, ',')) /* assumes field1,designation,field3 */
    *des = '\0';
  headfields = set_keys(heading_keys, &nheadfields);
  
  if (!(items = pg_load(ip, &nitems)))
    return 1;
  
  if (NULL == (pitems = pg_sort(ip, items, &nitems, perm_keys)))
    return 1;

  if (nitems)
    pages = pg_page(ip, pitems, nitems);

  if (iss_data(ip, pages))
    return 1;

  if (!strcmp(ip->show, "rref"))
    {
      if (iss_texts(ip, pages))
	return 1;
    }
  
  hash_free2(seen, free, NULL);

  return 0;
}
