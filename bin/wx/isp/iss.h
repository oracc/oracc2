#ifndef ISPSORT_H_
#define ISPSORT_H_

#include "isp.h"

struct item
{
  unsigned char   *s;
  unsigned char   *pq;
  unsigned char   *qpq; /* qualified pq */
  Unsigned32   skey;
  Signed32 lkey;
  Signed32 grp;
};

struct outline
{
  struct si_cache *sic;
  char *hdr;
  Unsigned32 *poffsets;
  Unsigned32 *icounts;
  Signed32 page;
  Signed32 count;
};

struct page
{
  Signed32 used;
  char **p;
};

extern int sic_size;
extern int *sortfields;
extern struct si_cache **sicache;
extern struct sortinfo *sip;

extern int nheadfields;
extern int *headfields;
extern int nsortfields;

#endif/*ISPSORT_H_*/
