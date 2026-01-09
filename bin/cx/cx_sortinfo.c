#include <oraccsys.h>
#include "cx.h"

static FILE *sifp;

static void
cx_si_fields(Cx *c)
{
  fprintf(sifp, "#nfields %d\n", c->k->nfields);
  fprintf(sifp, "#nmapentries %d\n", c->k->nmapentries);
  int i;
  for (i = 0; c->k->fields[i]; ++i)
    {
      KD_map *mp = hash_find(c->k->sortable, (uccp)c->k->fields[i]);
      fprintf(sifp, "#field ");
      const char *f;
      for (f = list_first(mp->fields); f; list_next(mp->fields))
	fprintf(sifp, "%s ", f);
      fprintf(sifp, "= %s\n", mp->hr);
    }
}

void
cx_sortinfo(Cx *c)
{
  sifp = stdout;
  cx_si_fields(c);
}
