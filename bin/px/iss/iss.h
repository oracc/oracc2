#ifndef ISS_H_
#define ISS_H_


#include "../isp/isp.h"
#include "iss_sortinfo.h"
#include "iss_redblack.h"

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

extern int iso_master(Isp *ip);
extern int iso_zoutline(Isp *ip);

#include "all.proto"

#endif/*ISS_H_*/
