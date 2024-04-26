#ifndef PUI_H_
#define PUI_H_

#include "../isp/isp.h"

struct puifilestab
{
  const char *name;
  const char **text;
};

extern const char *p4pager;
extern const char *p4error;

extern const char *pui_filetext(const char *file);
extern void pui_output(Isp *ip, FILE *fp, const char *s);
extern struct puifilestab *puifiles (register const char *str, register size_t len);

#endif/*PUI_H_*/
