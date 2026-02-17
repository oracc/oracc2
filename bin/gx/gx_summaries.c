#include <oraccsys.h>
#include <xnn.h>
#include <ns-cbd_summaries.h>
#include <joxer.h>
#include "gx.h"

static void o_jox_summaries(Cbd *c);
static void o_jox_letter(Letter *lp);

void
gx_summaries(Cbd *c)
{
  FILE *sfp = fopen("summaries.xml", "w");
  jox_xml_output(sfp);
  joxer_init(&cbd_summaries_data, "cbd-summaries", 1, sfp, NULL);
  o_jox_summaries(curr_cbd);
  joxer_term(sfp,NULL);
}

static void
o_jox_summaries(Cbd *c)
{
  int l;
  for (l = 0; l < c->nletters; ++l)
    o_jox_letter(&c->letters[l]);
}

static void
o_jox_letter(Letter *lp)
{
}
