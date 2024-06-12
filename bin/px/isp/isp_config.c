#include <oraccsys.h>
#include <xpd.h>
#include "../pxdefs.h"
#include "isp.h"

#define SORT_LABELS "Time,Genre,Place"
#define SORT_FIELDS "period,genre,provenience"

int
isp_config(Isp *ip)
{
#undef d
#undef s
#define d(fld,key) if ((tmp=hash_find(ip->xpd->opts,(ucp)(key)))) ip->default_cfg.fld = tmp
#define s(fld,key) if ((tmp=hash_find(ip->xpd->opts,(ucp)(key)))) ip->special_cfg.fld = tmp
  const char *tmp;
  
  const char *sel = NULL;

  static struct isp_config defaults =
    {
      .cat_fields="designation,primary_publication,subgenre|genre,period,place|provenience",
      .cat_widths="auto,17,17,17,17",
      .sort_fields=NULL,
      .sort_labels=NULL,
      .head_template="1 2 3"
    };

  ip->default_cfg = ip->special_cfg = defaults;
  
  ip->default_cfg.leftmenu = ip->special_cfg.leftmenu = 1;

  if ((sel = hash_find(ip->xpd->opts, (ucp)"outline-default-leftmenu"))
      && (!strcmp(sel, "false")|| !strcmp(sel, "no")))
    ip->default_cfg.leftmenu = 0;

  if ((sel = hash_find(ip->xpd->opts, (ucp)"outline-special-leftmenu"))
      && (!strcmp(sel, "false")|| !strcmp(sel, "no")))
    ip->default_cfg.leftmenu = 0;
  
  if ((sel = hash_find(ip->xpd->opts, (ucp)"outline-default-select")))
    {
      if (!strcmp(sel, "true")|| !strcmp(sel, "yes"))
	ip->default_cfg.select = 1;
    }
  else
    ip->default_cfg.select = 1;

  d(cat_fields,"outline-default-catalog-fields");
  d(cat_widths,"outline-default-catalog-widths");
  d(sort_fields,"outline-default-sort-fields");
  d(sort_labels,"outline-default-sort-labels");
  d(head_template,"outline-default-sort-template");  

  if (!ip->default_cfg.sort_fields)
    {
      ip->default_cfg.sort_fields = SORT_FIELDS;
      if (!ip->default_cfg.sort_labels)
	ip->default_cfg.sort_labels = SORT_LABELS;
    }
  else if (!ip->default_cfg.sort_labels)
    ip->default_cfg.sort_labels = ip->default_cfg.sort_fields;
  
  if ((sel = hash_find(ip->xpd->opts, (ucp)"outline-special-select")))
    if (!strcmp(sel, "true")|| !strcmp(sel, "yes"))
      ip->special_cfg.select = 1;

  s(cat_fields,"outline-special-catalog-fields");
  s(cat_widths,"outline-special-catalog-widths");
  s(sort_fields,"outline-special-sort-fields");
  s(sort_labels,"outline-special-sort-labels");
  s(head_template,"outline-special-sort-template");

  if (!ip->special_cfg.sort_fields)
    {
      ip->special_cfg.sort_fields = SORT_FIELDS;
      if (!ip->special_cfg.sort_labels)
	ip->special_cfg.sort_labels = SORT_LABELS;
    }
  else if (!ip->special_cfg.sort_labels)
    ip->special_cfg.sort_labels = ip->special_cfg.sort_fields;
  

  ip->default_cfg.cat_links = hash_find(ip->xpd->opts, (ucp)"catalog-link-fields");

#undef d
#undef s

  /* In P4 'special' outline mode is only used when the list name is outlined.lst */
  if (ip->list_name && !strcmp(ip->list_name, "outlined.lst") && ip->special_cfg.select)
    {
      ip->curr_cfg = &ip->special_cfg;
      ip->dors = "1";
    }
  else
    {
      ip->curr_cfg = &ip->default_cfg;
      ip->dors = "0";
    }

  isp_config_perm(ip);
  
  return 0;
}

int
isp_config_perm(Isp *ip)
{
  const char *sort_keys = ip->curr_cfg->sort_fields;

  if (!sort_keys)
    {
      sort_keys = "period,subgenre,provenience";
      ip->zlev = 3;
    }
  else
    {
      ip->zlev = 1;
      const char *k = sort_keys;
      while (*k)
	if (',' == *k++)
	  ++ip->zlev;
    }

  if (!ip->perm || '#' == *ip->perm)
    {
      switch(ip->zlev)
	{
	case 3:
	  ip->perm = "123";
	  break;
	case 2:
	  ip->perm = "12";
	  break;
	default:
	  ip->perm = "1";
	  break;
	}
    }

  return 0;
}
