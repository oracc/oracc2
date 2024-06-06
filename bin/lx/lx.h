#ifndef LX_H_
#define LX_H_

/* Parse structure for tcma/ali1:P123456@tcma */
typedef struct lx
{
  char *p;	/*project  */
  char *i;	/*item, PQX*/
  char *c;	/*catalogue*/
  char *x;	/*extra data; ignored but passed through */
  int proxy;	/* item came from a proxy */
  int l;       	/*line num*/
} Lx;

typedef struct lxfile
{
  const char *fn;
  char *fmem;
  char **lines;
  size_t nlines;
  Lx *items;
  size_t nitems;
  Hash *seen;
} Lxfile;

extern int check;
extern char *metaforce;
extern const char *output;
extern const char *prog;
extern const char *project;
extern int proxy;
extern int qualify;
extern int sort;
extern int status;
extern int uniq;
extern int verbose;
extern int ximport;
extern int all_optional;
extern int zfirstoptional;

#include "all.proto"

#endif/*LX_H_*/
