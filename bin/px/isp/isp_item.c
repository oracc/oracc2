#include <oraccsys.h>
#include "../pxdefs.h"
#include "isp.h"

void
isp_item_undump(char *k, struct isp_itemdata *idp)
{
  idp->index = k;
  char *s = strchr(idp->index, '\t');
  *s++ = '\0';

  idp->page = s;
  s = strchr(idp->page, '\t');
  *s++ = '\0';

  idp->pindex = s;
  s = strchr(s, '\t');
  *s++ = '\0';
  
  idp->zoom = s;
  s = strchr(s, '\t');
  *s++ = '\0';
  
  idp->zpag = s;
  s = strchr(s, '\t');
  *s++ = '\0';

  idp->zindex = s;
  s = strchr(s, '\t');
  *s++ = '\0';

  if ('#' == *s)
    idp->prev = NULL;
  else
    idp->prev = s;
  s = strchr(s, '\t');
  *s++ = '\0';

  if ('#' == *s)
    idp->next = NULL;
  else
    idp->next = s;
  s = strchr(s, '\t');
  *s++ = '\0';

  idp->proj = s; 
}

int
isp_item_load(Isp *ip)
{
  char *tmp = (char*)pool_copy((ucp)ip->item, ip->p);
  ip->itemdata.item = tmp;

  tmp = strchr(ip->itemdata.item, '_');
  if (tmp)
    {
      ip->itemdata.fullitem = (ccp)pool_copy((ucp)ip->itemdata.item, ip->p);
      *tmp = '\0';
    }
  else
    {
      tmp = strchr(ip->itemdata.item, '.');
      if (tmp)
	{
	  ip->itemdata.dotted = (ccp)pool_copy((ucp)ip->itemdata.item, ip->p);
	  *tmp = '\0';
	}
    }

  const char *sort = ip->cache.sort;

  if (!strcmp(ip->show, "rref"))
    {
      sort = ip->cache.t_sort;
      ip->itemdata.hili = (ccp)isp_hilited(ip);
    }
  else if (ip->itemdata.dotted)
    ip->itemdata.hili = ip->itemdata.dotted;
  
  Dbi_index *dp;
  if ((dp = dbx_init(sort, "itm")))
    {
      char *tmax = NULL;
      char *k = (char*)pool_copy((ucp)dbx_key(dp, ip->itemdata.item, NULL), ip->p);
      if (k)
	{
	  if ((tmax = (char*)dbx_key(dp, "tmax", NULL)))
	    ip->itemdata.tmax = (ccp)pool_copy((ucp)tmax, ip->p);
	  dbx_term(dp);
	}
      else
	{
	  dbx_term(dp);
	  /* If current list is outlined.lst and there is also a cache
	     dir txtindex.lst try that before giving up.  This ensures
	     that exemplars in projects that have only composites in
	     outlined.lst will be found when they are accessed via
	     score blocks */
	  if (!strcmp(ip->list_name, "outlined.lst"))
	    {
	      char t[strlen(ip->cache.sub)+strlen("/1230")];
	      strcpy(t, ip->cache.sub);
	      char *slash = strrchr(t, '/');
	      sprintf(slash+1, "txtindex.lst/123");
	      struct stat sb;
	      if (stat(t, &sb) == 0 && S_ISDIR(sb.st_mode))
		{
		  if ((dp = dbx_init(t, "itm")))
		    {
		      k = (char*)pool_copy((ucp)dbx_key(dp, ip->itemdata.item, NULL), ip->p);
		      if (k)
			{
			  ip->cache.txtindex = (char*)pool_copy((ucp)t, ip->p);
			  if ((tmax = (char*)dbx_key(dp, "tmax", NULL)))
			    ip->itemdata.tmax = (ccp)pool_copy((ucp)tmax, ip->p);
			  goto ok;
			}
		      dbx_term(dp);
		    }
		}
	    }
	  ip->err = PX_ERROR_START "key %s not found in item db\n";
	  ip->errx = ip->itemdata.item;
	  return 1;
	}
    ok:
      isp_item_undump(k, &ip->itemdata);
      ip->zoom = ip->itemdata.zoom;
      ip->page = ip->itemdata.page;
      if (set_item_max(ip))
	return 1;
    }
  else
    ip->err = (ccp)px_err("failed to open dbi index %s::%s\n", ip->cache.sort, "itm");

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
isp_create_cfy(Isp *ip)
{
  List *args = list_create(LIST_SINGLE);
  list_add(args, (void*)ip->oracc);
  list_add(args, (void*)"/bin/ispcfy.sh");
  list_add(args, " ");
  list_add(args, (void*)(ip->itemdata.proj ? ip->itemdata.proj : ip->project));
  list_add(args, " ");
  list_add(args, (void*)ip->itemdata.item);
  list_add(args, " ");
  list_add(args, (void*)ip->cache.prox);
  list_add(args, " ");
  list_add(args, (void*)ip->cache.out);
  unsigned char *syscmd = list_concat(args);

  if (ip->verbose)
    fprintf(stderr, "isp: isp_create_cfy: %s\n", syscmd);

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
	  ip->err = PX_ERROR_START "isp_create_cfy failed system call:\n\t%s\n";
	  ip->errx = (ccp)syscmd;
	  return 1;
	}
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
  list_add(args, (void*)ip->cache.out);
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

int
isp_item_xtf(Isp *ip)
{
  if (!ip->itemdata.xmdxsl)
    if (!(ip->itemdata.xmdxsl = isp_xmd_outline(ip)))
      return 1;

  /* prx db is created from atf-data.tab and contains entries for
     all texts, not only proxied ones */
  ip->itemdata.proj =
    isp_dbx_one_off(ip, ip->project, "02pub", "prx", dotless(ip->itemdata.item), NULL);
  /* This can error but we keep going anyway because if there's no
     ATF but the text is in the catalogue we want to go ahead and
     create meta.xml */

  ip->itemdata.bld = expand(ip->itemdata.proj ? ip->itemdata.proj : ip->project,
			    dotless(ip->itemdata.item), NULL);
  struct stat st;
  stat(ip->itemdata.bld, &st);
  if (!S_ISDIR(st.st_mode))
    {
      ip->err = PX_ERROR_START "item bld directory %s not found or not readable\n";
      ip->errx = ip->itemdata.bld ? ip->itemdata.bld : "(null)";
      return 1;
    }
  
  ip->itemdata.xtflang = isp_item_lang(ip);

  isp_htmd(ip);
	   
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
    {
      if (isp_create_xtf(ip))
	return 1;
      else if (ip->err) /* ispxtf succeeded so clear any prx db error */
	ip->err = ip->errx = NULL;
    }

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

  if (isp_item_langs(ip))
    return 1;
  
  if (isp_item_xtf(ip))
    return 1;

  return 0;
}
 
int
isp_item_cfy(Isp *ip)
{
  isp_htmd(ip);
	   
  expand_base(ip->itemdata.htmd);

  /* always use host project for xmd, never proxy; cache.item is only used for meta.xml */
  ip->cache.item = (ccp)pool_copy((ucp)expand(ip->project, ip->item, NULL), ip->p);
 
  if (ip->itemdata.proj && strcmp(ip->project, ip->itemdata.proj))
    ip->cache.prox = (ccp)pool_copy((ucp)expand(ip->itemdata.proj, ip->item, NULL), ip->p);
  else
    ip->cache.prox = ip->cache.item;
  
  char *cfy = expand(ip->itemdata.proj ? ip->itemdata.proj : ip->project, ip->item, "cfy");
  expand_base(NULL);

  /* +5 here is lang which can be 2 or 3, '.' before lang and \0 at end */
  char *xh = (char*)pool_alloc(strlen(cfy)+5,ip->p);
  strcpy(xh, cfy);
  ip->itemdata.html = ip->itemdata.cfy = xh;

  fprintf(stderr, "itemdata.cfy = %s\n", ip->itemdata.cfy);
  
  int need_html = access(ip->itemdata.cfy, R_OK);

  if (need_html)
    {
      if (isp_create_cfy(ip))
	return 1;
      else if (ip->err) /* ispcfy succeeded so clear any prx db error */
	ip->err = ip->errx = NULL;
    }
  
  return 0;
}
 
 
static int
isp_create_img(Isp *ip)
{
  char nodot[8];
  List *args = list_create(LIST_SINGLE);
  list_add(args, (void*)ip->oracc);
  list_add(args, (void*)"/bin/ispimg.sh");
  list_add(args, " ");
  if (strchr(ip->itemdata.item, '.'))
    {
      strncpy(nodot,ip->itemdata.item,7); nodot[7] = '\0';
      list_add(args, nodot);
    }
  else
    list_add(args, (void*)ip->itemdata.item);
  list_add(args, " ");
  list_add(args, (void*)ip->itemdata.img);
  list_add(args, " ");
  list_add(args, (void*)ip->cache.out);
  unsigned char *syscmd = list_concat(args);

  if (ip->verbose)
    fprintf(stderr, "isp: isp_create_img: %s\n", syscmd);

  int sys;
  if ((sys = system((ccp)syscmd)))
    {
      if (sys >= 0)
	{
	  int xstatus = WEXITSTATUS(sys);
	  /*int ifexited = WIFEXITED(sys);*/
	  if (xstatus == 127)
	    {
	      ip->err = PX_ERROR_START "non-existent command:\n\t%s\n";
	      ip->errx = (ccp)syscmd;
	      return 1;
	    }
	  else if (xstatus > 1)
	    {
	      ip->itemdata.not = xstatus;
	    }
	  else
	    {
	      ip->err = PX_ERROR_START "isp_create_img failed system call:\n\t%s\n";
	      ip->errx = (ccp)syscmd;
	      return 1;
	    }
	}
      else
	{
	  ip->err = PX_ERROR_START "isp_create_img failed system call failed:\n\t%s\n";
	  ip->errx = (ccp)syscmd;
	  return 1;
	}
	    
    }
  
  return 0;
}

int
isp_item_img(Isp *ip)
{
  char img[strlen(ip->cache.sys)+strlen("/img/P123/P123456.img0")];
  char four[5]; strncpy(four,ip->item, 4); four[4] = '\0';
  sprintf(img, "%s/img/%s/%s.img", ip->cache.sys, four, ip->item);
  ip->itemdata.img = (ccp)pool_copy((uccp)img, ip->p);
  if (access(img, R_OK))
    return isp_create_img(ip);
  return 0;
}

