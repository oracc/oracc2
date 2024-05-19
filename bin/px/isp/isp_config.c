#include <oraccsys.h>
#include <xpd.h>
#include "../px.h"
#include "../pxdefs.h"
#include "isp.h"

int
isp_config(Isp *ip)
{
#undef d
#undef s
#define d(fld,key) ip->default_cfg.fld=hash_find(ip->xpd->opts,(ucp)(key))
#define s(fld,key) ip->special_cfg.fld=hash_find(ip->xpd->opts,(ucp)(key))

  const char *sel = NULL;

  if ((sel = hash_find(ip->xpd->opts, (ucp)"outline-default-select")))
    if (!strcmp(sel, "true")|| !strcmp(sel, "yes"))
      ip->default_cfg.select = 1;
  d(cat_fields,"outline-default-catalog-fields");
  d(cat_widths,"outline-default-catalog-widths");
  d(sort_fields,"outline-default-sort-fields");
  d(sort_fields,"outline-default-sort-fields");
  d(sort_labels,"outline-default-sort-labels");
  d(head_template,"outline-default-sort-template");

  if ((sel = hash_find(ip->xpd->opts, (ucp)"outline-special-select")))
    if (!strcmp(sel, "true")|| !strcmp(sel, "yes"))
      ip->special_cfg.select = 1;
  s(cat_fields,"outline-special-catalog-fields");
  s(cat_widths,"outline-special-catalog-widths");
  s(sort_fields,"outline-special-sort-fields");
  s(sort_fields,"outline-special-sort-fields");
  s(sort_labels,"outline-special-sort-labels");
  s(head_template,"outline-special-sort-template");

  ip->default_cfg.cat_links = hash_find(ip->xpd->opts, (ucp)"catalog-link-fields");

#undef d
#undef s

  return 0;
}
