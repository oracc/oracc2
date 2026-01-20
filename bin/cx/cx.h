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
  struct keydata *k;
  Memo *msort;
  Pool *si_pool; /* pool exclusively for sortinfo */
  Pool *p; /* general purpose pool */
  struct xpd *cfg;
  Roco *rr[7];
  Fcell **sirr[7];
} Cx;

struct cx_xml_user
{
  Cx *c;
  Fcell **f;
};

typedef struct sis
{
  int r; /* remember which Roco this sis comes from; the number of
	    fields in keydata is the same for all roco, but the
	    maxcols value can vary among the different roco */
  long l;
  Fcell *f;
} Sis;

#define LM_QQQ  0x01000000
#define LM_SUX  0x02000000
#define LM_AKK  0x08000000
#define LM_PCUN 0x10000000
#define LM_PELM 0x20000000
#define LM_MISC 0x80000000

typedef const char * (*Repfunc)(const char *s);

extern int merge_fields, sortinfo_only, verbose;

struct merper { const char *name; const char *merge; };
extern struct merper * cx_merper(const char *s, size_t len);
extern void cx_roco(Cx *cp);
extern int cx_roco_id_index(Roco *r);
extern void cx_keydata(Cx *c);
extern int cx_remap(Cx *c);
extern void cx_sortinfo(Cx *c);
extern int cx_roco_field_index(Roco *r, const char *fld);
extern void cx_merge_periods(Roco *r);
extern void cx_replace(Roco *r, const char *field, Repfunc repfunc);
extern Fcell **cx_si_marshall(Roco *r);

#endif/*CX_H_*/
