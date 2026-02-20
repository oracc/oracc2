#include <oraccsys.h>
#include <joxer.h>
#include <rnvif.h>
#include <xmlify.h>
#include <pool.h>
#include <joxer.h>
#include <xnn.h>
#include <ns-xtf.h>
#include <rnvxml.h>
#include <gt.h>
#include <asl.h>
#include "ax.h"

Mloc *xo_loc;

const char *xfn = NULL;
const char *file;

static void
axj_atf(ATF *a)
{
  joxer_ea(xo_loc, "xtf", NULL);
  joxer_ee(xo_loc, "xtf");
}

void
ax_jox(ATF*a)
{
  if (!xfn)
    xfn = "ax.xml";

  FILE *xfp = fopen(xfn, "w");

  jox_xml_output(xfp);
  joxer_init(&xtf_data, "xtf", 1, xfp, NULL);
  xo_loc = malloc(sizeof(Mloc));
  xo_loc->file = file = a->file;
  xo_loc->line = 1;
  axj_atf(a);
  joxer_term(xfp,NULL);
}

