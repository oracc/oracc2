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
  Dbi_index *dp = dbx_init(ip->cache.sub, dbifn);
  char *k = (char*)dbx_key(dp, ip->itemdata.item, NULL);
  if (!k)
    {
      ip->err = PX_ERROR_START "key %s not found in item db\n";
      ip->errx = ip->itemdata.item;
      return 1;
    }

  ip->itemdata.page = (ccp)pool_copy((ucp)k, ip->p);
  
  char *s = strchr(ip->itemdata.page, '\t');
  *s++ = '\0';

  ip->itemdata.zoom = s;
  s = strchr(s, '\t');
  *s++ = '\0';
  
  ip->itemdata.zpag = s;
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
  
  return 0;
}

static int
isp_item_langs(Isp *ip)
{
  size_t n;
  char langsfile[strlen(ip->projdir)+strlen("/02pub/langs.txt0")];
  sprintf(langsfile, "%s/02pub/langs.txt", ip->projdir);
  if (!(ip->itemdata.langs = (const char **)px_loadfile_lines3((ucp)langsfile,
							       &n, (unsigned char **)&ip->itemdata.lmem)))
    {
      ip->err = (ccp)px_loadfile_error();
      return 1;
    }
  return 0;
}

/* Ensure there is a lang in scope for the current xtf-item; we test
 * for the existence of a .xtr file and if there are any translations
 * at all we return the lang of an existing translation if the
 * requested language is not available.
 */
static const char *
isp_item_lang(Isp *ip)
{
  const char *first_lang = NULL;
  if (ip->lang && ip->itemdata.langs && ip->itemdata.langs[0])
    {
      int i;
      for (i = 0; ip->itemdata.langs[i]; ++i)
	{
	  if (!strcmp(ip->lang, ip->itemdata.langs[i]))
	    {
	      const char *fn = expand_xtr(ip->project, ip->itemdata.item, "project", ip->lang);
	      if (!access(fn, R_OK))
		return ip->lang;
	    }
	  else if (!first_lang)
	    {
	      const char *fn = expand_xtr(ip->project, ip->itemdata.item, "project", ip->itemdata.langs[i]);
	      if (!access(fn, R_OK))
		first_lang = ip->itemdata.langs[i];
	    }
	}
      if (first_lang)
	return first_lang;
      else
	return "en";
    }
  else if (ip->lang)
    return "en";
  else
    return ip->itemdata.langs[0];
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
  list_add(args, (void*)ip->itemdata.item);
  list_add(args, " ");
  list_add(args, (void*)ip->cache.item);
  unsigned char *syscmd = list_concat(args);
  if (ip->verbose)
    fprintf(stderr, "isp: isp_create_xtf: %s\n", syscmd);

  if (system((ccp)syscmd))
    {
      ip->err = PX_ERROR_START "isp_create_xtf failed system call:\n\n\t%s\n";
      ip->errx = (ccp)syscmd;
      return 1;
    }
  
  return 0;
}


static int
isp_item_xtf(Isp *ip)
{
  const char *xtflang = isp_item_lang(ip);
  char *html = expand(ip->project, ip->item, "html");

  char itemcache[strlen(ip->cache.sub)+strlen(ip->item)+2];
  sprintf(itemcache, "%s/%s", ip->cache.sub, ip->item);
  ip->cache.item = (ccp)pool_copy((ucp)itemcache, ip->p);

  ip->cache.meta = (ccp)pool_alloc(strlen(itemcache)+strlen("/meta.xml0"), ip->p);
  sprintf((char*)ip->cache.meta, "%s/meta.xml", itemcache);

  ip->itemdata.html = (ccp)pool_alloc(strlen(itemcache)+strlen(html)+strlen(xtflang)+3, ip->p);
  sprintf((char*)ip->itemdata.html, "%s/%s", itemcache, html);
  if (strcmp(xtflang, "en"))
    sprintf((char*)(ip->itemdata.html+strlen(ip->itemdata.html)), ".%s", xtflang);
  
  int need_html = access(ip->itemdata.html, R_OK);
  int need_meta = access(ip->itemdata.html, R_OK);

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

  ip->itemdata.bld = expand(ip->project, ip->itemdata.item, NULL);
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

