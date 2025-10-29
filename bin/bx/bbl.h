#ifndef BBL_H_
#define BBL_H_

#include <mesg.h>
#include <hash.h>

typedef struct bbl
{
  const char *basename;
  List *elist;
  struct bblentry **eptrs;
} Bbl;

typedef struct bblentry
{
  const char *ref;
  Hash *fields;
  int index;
  const char *h4t_ref;
  const char *h4t_bib;
} Bblentry;

extern void bbl_load(const char *bblfile);

#endif/*BBL_H_*/
