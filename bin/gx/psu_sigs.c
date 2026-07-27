#include <oraccsys.h>
#include <cbd.h>
#include "gx.h"

#if 0
static void
psu_ngms_list(List *sigs, MWE_type mt)
{
  char *sig;
  int psu_sense_rank = 1;
  for (sig = list_first(sigs); sig; sig = list_next(sigs))
    if (out_stdout)
      fprintf(stdout, "%s\n", sig);
    else
      cbd_sig_add_one((uccp)sig, psu_sense_rank-- > 0? 1 : 0, mt);
}
#endif

static void
psu_sigs_list(List *sigs, MWE_type mt)
{
  char *sig;
  int psu_sense_rank = 1;
  for (sig = list_first(sigs); sig; sig = list_next(sigs))
    if (out_stdout)
      fprintf(stdout, "%s\n", sig);
    else
      cbd_sig_add_one((uccp)sig, psu_sense_rank-- > 0 ? 1 : 0, mt);
}

void
psu_ngms(Parts *p)
{
  Entry *ep = p->owner;
  Sense *sp;
  int psu_sense_rank = 1;
  for (sp = list_first(ep->senses); sp; sp = list_next(ep->senses))
    {
      char *buf = (char*)pool_alloc(strlen((ccp)p->ngram)+strlen((ccp)sp->cgspe)+strlen(" +=  0"),
				    csetp->pool);
      sprintf(buf, "%%%s:%s += %s", p->owner->lang, p->ngram, sp->cgspe);
      cbd_sig_add_one((uccp)buf, psu_sense_rank-- > 0 ? 1 : 0, ep->usage ? M_MWE_NGM : M_PSU_NGM);
    }
}

void
psu_sigs(Entry *ep)
{
  Parts *p;
  for (p = list_first(ep->parts); p; p = list_next(ep->parts))
    psu_ngms(p);

  Cform *cfp;
  for (cfp = list_first(ep->forms); cfp; cfp = list_next(ep->forms))
    {
      List *sigs = cbd_psu_sigs(&cfp->f);
      if (sigs)
	psu_sigs_list(sigs, ep->usage ? M_MWE_SIG : M_PSU_SIG);
    }
}
