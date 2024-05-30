#include <oraccsys.h>
#include "../px.h"
#include "isp.h"

int
isp_item_load(Isp *ip)
{
  char *tmp = (char*)pool_copy((ucp)ip->item, ip->p);
  ip->itemdata.item = tmp;

  tmp = strchr(ip->itemdata.item, '.');
  if (tmp)
    {
      *tmp++ = '\0';
      ip->itemdata.dots = tmp;
    }
  
  char dbifn[strlen(ip->perm)+strlen("-itm0")];
  sprintf(dbifn, "%s-itm", ip->perm);
  Dbi_index *dp;
  if ((dp = dbx_init(ip->cache.sub, dbifn)))
    { 
      char *tmax = (char*)dbx_key(dp, "tmax", NULL);
      if (tmax)
	ip->itemdata.tmax = (ccp)pool_copy((ucp)tmax, ip->p);
      char *k = (char*)pool_copy((ucp)dbx_key(dp, ip->itemdata.item, NULL), ip->p);
      dbx_term(dp);

      if (!k)
	{
	  ip->err = PX_ERROR_START "key %s not found in item db\n";
	  ip->errx = ip->itemdata.item;
	  return 1;
	}

      ip->itemdata.index = (ccp)pool_copy((ucp)k, ip->p);
      char *s = strchr(ip->itemdata.index, '\t');
      *s++ = '\0';

      ip->itemdata.page = s;
      s = strchr(ip->itemdata.page, '\t');
      *s++ = '\0';

      ip->zoom = ip->itemdata.zoom = s;
      s = strchr(s, '\t');
      *s++ = '\0';
  
      ip->page = ip->itemdata.zpag = s;
      s = strchr(s, '\t');
      *s++ = '\0';

      ip->itemdata.zindex = s;
      s = strchr(s, '\t');
      *s++ = '\0';

      if ('#' == *s)
	ip->itemdata.prev = NULL;
      else
	ip->itemdata.prev = s;
      s = strchr(s, '\t');
      *s++ = '\0';

      if ('#' == *s)
	ip->itemdata.next = NULL;
      else
	ip->itemdata.next = s;
      s = strchr(s, '\t');
      *s++ = '\0';

      ip->itemdata.proj = s;
    }
  else
    ip->err = (ccp)px_err("failed to open dbi index %s/%s\n", ip->cache.sub, dbifn);

  return ip->err ? 1 : 0;
}

static int
isp_item_langs(Isp *ip)
{
  ip->itemdata.langs =
    isp_dbx_one_off(ip, ip->itemdata.proj ? ip->itemdata.proj : ip->project,
		    "02pub", "trs", ip->itemdata.item, NULL);
  
  if (ip->itemdata.langs)
    {
      ip->itemdata.langp =
	(const char **)vec_from_str((char*)pool_copy((ucp)ip->itemdata.langs, ip->p),
				    NULL, &ip->itemdata.nlangs);
    }
  else
    {
      /* No errors here because we build the trs db off the .atf */
      ip->err = NULL;
    }
  return 0;
}

static int
isp_create_xtf(Isp *ip)
{
  List *args = list_create(LIST_SINGLE);
  list_add(args, (void*)ip->oracc);
  list_add(args, (void*)"/bin/ispxtf.sh");
  list_add(args, " ");
  list_add(args, (void*)ip->project);
  list_add(args, " ");
  list_add(args, (void*)(ip->itemdata.proj ? ip->itemdata.proj : ip->project));
  list_add(args, " ");
  list_add(args, (void*)ip->itemdata.item);
  list_add(args, " ");
  list_add(args, (void*)(ip->itemdata.xtflang ? ip->itemdata.xtflang : "-"));
  list_add(args, " ");
  list_add(args, (void*)ip->cache.item);
  list_add(args, " ");
  list_add(args, (void*)ip->cache.prox);
  list_add(args, " ");
  list_add(args, (void*)ip->itemdata.xmdxsl);
  list_add(args, " ");
  list_add(args, (void*)ip->cache.sub);
  unsigned char *syscmd = list_concat(args);

  if (ip->verbose)
    fprintf(stderr, "isp: isp_create_xtf: %s\n", syscmd);

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
	  ip->err = PX_ERROR_START "isp_create_xtf failed system call:\n\t%s\n";
	  ip->errx = (ccp)syscmd;
	  return 1;
	}
    }
  
  return 0;
}

/* return the first char* in b that is in a or NULL if none
 *
 * Note that the codes in a, the translations list, may be 3-letter
 * and this needs to be preserved so that 'hun' in ETCSRI gets found
 * properly.  But the codes in b, the http-accept-language codes, have
 * been forced to 2-letter if possible.  So we need to enforce
 * 2-letter code comparison but possible 3-letter code return.
 */
const char *
first_lang(const char **a, const char **b)
{
  int i, j;
  for (i = 0; b[i]; ++i)
    for (j = 0; a[j]; ++j)
      if (!strcmp(b[i], lang32(a[j])))
	return a[j];
  return NULL;
}

static const char *
isp_item_lang(Isp *ip)
{
  if (ip->lang)
    return ip->lang;
  
  if (ip->itemdata.langp)
    {
      if (ip->halp)
	{
	  const char *fl = first_lang(ip->itemdata.langp, ip->halp);
	  return fl ? fl : "en";
	}
      else
	return ip->itemdata.langp[0];
    }
  return NULL;
}

static int
isp_item_xtf(Isp *ip)
{
  ip->itemdata.xtflang = isp_item_lang(ip);

  if (!ip->itemdata.htmd)
    {
      const char *p4cache = getenv("ORACC_P4_CACHE");
      
      if (p4cache)
	{
	  ip->itemdata.htmd = (ccp)pool_alloc(strlen(p4cache)+5, ip->p);
	  sprintf((char*)ip->itemdata.htmd, "%s/htm", p4cache);
	}
      else
	ip->itemdata.htmd = "/home/oracc/www/p4.d/htm";
    }

  expand_base(ip->itemdata.htmd);
  /* always use host project for xmd, never proxy; cache.item is only used for meta.xml */
  ip->cache.item = (ccp)pool_copy((ucp)expand(ip->project, ip->item, NULL), ip->p);
  ip->cache.meta = (ccp)pool_alloc(strlen(ip->cache.item)+strlen("/meta.xml0"), ip->p);

  if (ip->itemdata.proj && strcmp(ip->project, ip->itemdata.proj))
    ip->cache.prox = (ccp)pool_copy((ucp)expand(ip->itemdata.proj, ip->item, NULL), ip->p);
  else
    ip->cache.prox = ip->cache.item;
  
  sprintf((char*)ip->cache.meta, "%s/meta.xml", ip->cache.item);
  char *html = expand(ip->itemdata.proj ? ip->itemdata.proj : ip->project, ip->item, "html");
  expand_base(NULL);

  /* +5 here is lang which can be 2 or 3, '.' before lang and \0 at end */
  char *xh = (char*)pool_alloc(strlen(html)+5,ip->p);
  strcpy(xh, html);
  ip->itemdata.html = xh;
  if (ip->itemdata.xtflang && strcmp(ip->itemdata.xtflang, "en"))
    sprintf((char*)(ip->itemdata.html+strlen(ip->itemdata.html)), ".%s", ip->itemdata.xtflang);

  fprintf(stderr, "itemdata.html = %s\n", ip->itemdata.html);
  
  int need_html = access(ip->itemdata.html, R_OK);
  int need_meta = access(ip->cache.meta, R_OK);

  if (need_html || need_meta)
    if (isp_create_xtf(ip))
      return 1;

  return 0;
}

int
isp_item_set(Isp *ip)
{
  if (isp_item_load(ip))
    return 1;
  
  /* This implementation doesn't support creating an unzoomed pager from an item yet */
  ip->zoom = ip->itemdata.zoom;
  ip->page = ip->itemdata.zpag;

  if (!(ip->itemdata.xmdxsl = isp_xmd_outline(ip)))
    return 1;

  ip->itemdata.bld = expand(ip->itemdata.proj ? ip->itemdata.proj : ip->project,
			    ip->itemdata.item, NULL);
  struct stat st;
  stat(ip->itemdata.bld, &st);
  if (!S_ISDIR(st.st_mode))
    {
      ip->err = PX_ERROR_START "item bld directory %s not found or not readable\n";
      ip->errx = ip->itemdata.bld ? ip->itemdata.bld : "(null)";
      return 1;
    }

  if (isp_item_langs(ip))
    return 1;
  
  if (isp_item_xtf(ip))
    return 1;

  return 0;
}

