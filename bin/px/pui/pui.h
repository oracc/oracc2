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

extern const char *p4pager, *p4error, *p4controls, *p4footer, *p4search,
  *p4icons, *p4html, *p4head, *p4dropdown, *p4debug, *p4debugdiv, *p4itemxtf;

extern const char *pui_filetext(const char *file);
extern int pui_output(Isp *ip, FILE *fp, const char *s);
extern struct puifilestab *puifiles(register const char *str, register size_t len);
extern struct atactionstab *atactions(register const char *str, register size_t len);

extern atfunc pui_at_error, pui_at_invocation, pui_at_status,
  pui_at_state, pui_at_title, pui_at_menu, pui_at_content, pui_at_project,
  pui_at_translations, pui_at_current_page, pui_at_active_pages, pui_at_active_items,
  pui_at_srch_results, pui_at_ceid, pui_at_environment, pui_at_pager_data,
  pui_at_item_data, pui_at_item_meta
  ;

#endif/*PUI_H_*/
