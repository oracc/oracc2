#ifndef LX_H_
#define LX_H_

/* Parse structure for tcma/ali1:P123456@tcma */
typedef struct lx
{
  char *p;	/*project  */
  char *i;	/*item, PQX*/
  char *c;	/*catalogue*/
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
extern int output;
extern const char  *project;
extern int qualify;
extern int sort;
extern int uniq;
extern int verbose;

#include "all.proto"

#endif/*LX_H_*/
