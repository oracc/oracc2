#include <oraccsys.h>
#include <l3.h>
#include "ax.h"

void
ax_lem(Run *run, XCL *xcp)
{
  int pre_lem_status = status; /* lem errors don't suppress output */
  if (xcp->root)
    {
      struct sig_context *scp = NULL;
      if (!xcp->linkbase)
	{
	  xcp->linkbase = new_linkbase();
	  xcp->linkbase->textid = xcp->textid;
	}

      scp = xcp->sigs;
      scp->xpd = run->proj->xpd;
      scp->xcp = xcp;
      sig_context_langs(scp,xcp->langs);
	      
      if (lem_autolem)
	sig_new(xcp);
      else
	sig_check(xcp);

      if ((lem_autolem || lem_dynalem) && !lem_forms_raw)
	{
	  collos(xcp);
	}

      if ((lem_autolem || lem_dynalem) && !lem_forms_raw)
	bigrams(xcp);

#if 0
      if (lem_extended)
	xcl_map(xcp,NULL,NULL,NULL,xli_mapper);
#endif

      xcl_map(xcp,xcl_sentence_labels,NULL,NULL,NULL);

	      
      /* PSU matching must take place while the lists of
	 candidate matches are still available to prevent
	 match-failures or mismatches like mar shipri
	 matching shipru[//work] */
      if (!lem_autolem && !lem_dynalem)
	psus2(xcp);

      if (lem_do_wrapup)
	xcl_map(xcp,NULL,NULL,NULL,ilem_wrapup);

#if 0
      /* Previously, PSUs have had a disambiguating function, 
	 but if we place them after the ilem_wrapup this is
	 no longer true.  The alternative is that we must 
	 defer psu sig generation, moving it from psu processing
	 to post-ilem_wrapup somehow */
#endif

      /* from this point on the lemmatization is determined and
	 ->finds must always be NULL; this means that no further
	 disambiguation is performed by the recognizers that
	 follow */

      if (named_ents)
	psa_exec(xcp);

      if (perform_dsa)
	dsa_exec(xcp);

#if 0
      if (perform_nsa)
	nsa_xcl_input(xcp, global_nsa_context);
#endif

      if (harvest_notices)
	xcl_map(xcp,NULL,NULL,NULL,ilem_harvest_notices);

      if (warn_unlemmatized)
	xcl_map(xcp,NULL,NULL,NULL,ilem_unlemm_warnings);

      xcl_chunkify(xcp);
    }
  status = pre_lem_status;
}
