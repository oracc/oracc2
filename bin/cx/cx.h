#ifndef CX_H_
#define CX_H_

#include <roco.h>
#include <xpd.h>

struct keydata;
#define FCELL_STRP 0
#define FCELL_SORT 1
typedef struct fcell
{
  int type;
  union
  {
    const char *str;
    size_t index;
  } u;
  size_t sort;  
} Fcell;

typedef struct fsort
{
  Fcell *cp;
  List *cells;
} Fsort;

typedef struct cx
{
  const char *project;
  Roco *r;
  struct keydata *k;
  Memo *msort;
  Fcell **si_rows;
  Pool *si_pool; /* pool exclusively for sortinfo */
  Pool *p; /* general purpose pool */
  struct xpd *cfg;
} Cx;

typedef const char * (*Repfunc)(const char *s);

extern int sortinfo_only;

struct merper { const char *name; const char *merge; };
extern struct merper * cx_merper(const char *s, size_t len);
extern void cx_roco(Cx *cp);
extern int cx_roco_id_index(Cx *c);
extern void cx_keydata(Cx *c);
extern int cx_remap(Cx *c);
extern void cx_sortinfo(Cx *c);
extern int cx_roco_field_index(Cx *c, const char *fld);
extern void cx_merge_periods(Cx *c);
extern void cx_replace(Cx *c, const char *field, Repfunc r);

#endif/*CX_H_*/
