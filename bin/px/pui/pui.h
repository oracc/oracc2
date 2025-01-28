#ifndef PUI_H_
#define PUI_H_

#include "../isp/isp.h"

typedef void (atfunc)(Isp *ip, FILE *fp);

struct atactionstab
{
  const char *name;
  atfunc *func;
};

struct puifilestab
{
  const char *name;
  const char **text;
};

extern const char *p4pager, *p4error, *p4controls, *p4footer, *p4search, *p4searchbar,
  *p4icons, *p4html, *p4head, *p4dropdown, *p4debug, *p4debugdiv, *p4itemglo,
  *p4itemxtf, *p4select, *p4ceselect, *p4unknown, *p4noxtf, *p4nosrch,
  *p4full, *p4plus, *p4nowhat, *p4whatnot, *p4back, *p4article,
  *p4noproject, *p4noitem, *p4nolang, *p4oracc;

extern const char *pui_filetext(const char *file);
extern int pui_output(Isp *ip, FILE *fp, const char *s);
extern struct puifilestab *puifiles(register const char *str, register size_t len);
extern struct atactionstab *atactions(register const char *str, register size_t len);

extern atfunc pui_at_error, pui_at_invocation, pui_at_status,
  pui_at_title, pui_at_menu, pui_at_content, pui_at_project,
  pui_at_translations, pui_at_current_page, pui_at_active_pages, pui_at_active_items,
  pui_at_srch_results, pui_at_srch_results_matches, pui_at_ceid, pui_at_environment, pui_at_pager_data,
  pui_at_item_data, pui_at_item_meta, pui_at_item_label, pui_at_srchterm, pui_at_pager_class,
  pui_at_item_index, pui_at_item_index_nth, pui_at_item_index_typ, pui_at_select_ce,
  pui_at_select_sort, pui_at_select_trans, pui_at_page_label, pui_at_item_qpqx, pui_what_not,
  pui_at_tmax, pui_at_tmax_texts, pui_at_article, pui_at_generator,
  pui_at_cuneify_bar, pui_at_onload, pui_at_item_cfy, pui_at_item_img, pui_at_cfy_fam;

extern int pui_123(Isp *ip, const char *perm, const char *cfg, FILE *fp);

#endif/*PUI_H_*/
