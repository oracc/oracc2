#ifndef PX_H_
#define PX_H_

#include "pxdefs.h"

#include <stdlib.h>
#include "xslt_if.h"
#include "isp/isp.h"
#include "pui/pui.h"

struct cgioptstab
{
  const char *name;
  int opt;
};

struct frag
{
  int nesting;
  const char *xid;
  struct xft *xp;
  FILE *fp;
};

#include "xftable.h"

#if 1
#include "all.proto"
extern int px_return(Isp *ip);
extern struct cgioptstab *cgiopts(register const char *str, register size_t len);
#else
extern struct cgioptstab *cgiopts(register const char *str, register size_t len);
extern struct ispargstab *pxargs (register const char *str, register size_t len);
extern int px_validate(Isp *ip);
extern int px_options(int argc, char **argv, Isp *ip);
extern unsigned char *px_loadfile(unsigned const char *fname, size_t *nbytes);
extern unsigned char **px_loadfile_lines3(unsigned const char *fname, size_t *nlines, unsigned char **fmem);
extern unsigned char *px_loadfile_error(void);

extern int px_file_copy(Isp *ip, const char *fn, const char *to);
extern char *CGI_decode_url(const char *p);

extern const char *px_mesg(const char *wpq, const char *fmt, ...);

extern int px_return_file(const char *form, const char *file);
extern int px_return(Isp *ip);
extern const char *fatalize(const char *e);
extern int selecter(const char *file, char * const*ids);

extern void px_print_ip(FILE *fp, Isp *ip);

extern const char **http_accept_language(const char *hal);
extern const char *lang32(const char *l);
#endif
#endif/*PX_H*/
