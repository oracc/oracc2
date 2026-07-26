#include <oraccsys.h>
#include <cbd.h>
#include "gx.h"

static void
psu_sigs_list(List *sigs)
{
  char *sig;
  int psu_sense_rank = 1;
  for (sig = list_first(sigs); sig; sig = list_next(sigs))
    if (out_stdout)
      fprintf(stdout, "%s\n", sig);
    else
      cbd_sig_add_one((uccp)sig, psu_sense_rank-- ? 1 : 0);
}

void
psu_sigs(Entry *ep)
{
  psu_ngms;
  Parts *p;
  for (p = list_first(ep->parts); p; p = list_next(ep->parts))
    {
      /* refactor as cbd_psu_ngms() */
      List *sigs = cbd_psu_sigs(&p->f);
      if (sigs)
	psu_sigs_list(sigs);
    }

  Cform *cfp;
  for (cfp = list_first(ep->forms); cfp; cfp = list_next(ep->forms))
    {
      List *sigs = cbd_psu_sigs(&cfp->f);
      if (sigs)
	psu_sigs_list(sigs);
    }
}
