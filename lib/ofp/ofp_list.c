#include <oraccsys.h>
#include "ofp.h"

void
ofp_list(Ofp *o, const char *l)
{
  char buf[strlen(l)+5];
  unsigned char *fmem, **lp;
  size_t nline;

  sprintf(buf, "%s.tsv", l);
  lp = loadfile_lines3((uccp)buf, &nline, &fmem);
  o->list = calloc(nline, sizeof(Ofp_list));
  o->h_list = hash_create(1024);
  int i;
  for (i = 0; i < nline; ++i)
    {
      Ofp_list *olp = &o->list[i];
      olp->l = (ccp)lp[i];
      char *t = strchr((ccp)lp[i], '\t');
      *t++ = '\0';
      olp->o = (ccp)t;
      olp->s = i;
      hash_add(o->h_list, (uccp)olp->o, olp);
    }
}