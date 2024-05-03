#ifndef PX_H_
#define PX_H_

#include "pxdefs.h"

#include <stdlib.h>
#include "isp/isp.h"
#include "pui/pui.h"

struct cgioptstab
{
  const char *name;
  int opt;
};

extern struct cgioptstab *cgiopts(register const char *str, register size_t len);
extern struct ispargstab *pxargs (register const char *str, register size_t len);
extern int px_validate(Isp *ip);
extern int px_options(int argc, char **argv, Isp *ip);
extern unsigned char *px_loadfile(unsigned const char *fname, size_t *nbytes);
extern unsigned char **px_loadfile_lines3(unsigned const char *fname, size_t *nlines, unsigned char **fmem);
extern unsigned char *px_loadfile_error(void);

extern int px_file_copy(Isp *ip, const char *fn, const char *to);

#endif/*PX_H*/
