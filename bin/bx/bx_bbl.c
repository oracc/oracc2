#include <oraccsys.h>
#include "bx.h"
#include "bbl.h"
#include "bbl.tab.h"

Bbl *bbl;
Memo *bblentry_m;

void
bbl_load(const char *bblfile)
{
  extern const char *curr_bbl;
  extern FILE *bblin;
  curr_bbl = bblfile;
  FILE *fp = fopen(bblfile, "r");
  bblentry_m = memo_init(sizeof(Bblentry),256);
  if (!fp)
    {
      fprintf(stderr, "can't read .bbl %s\n", bblfile);
      exit(1);
    }
  bblin = fp;
  bblparse();
  fclose(fp);
}

void
bbl_datalist(Mloc m, const char *tok)
{
  if (verbose)
    fprintf(stderr, "bbl_datalist: tok=%s\n", tok);
  bbl = calloc(1, sizeof(Bbl));
  bbl->file = bblfile;
  bbl->type = tok;
  bbl->ehash = hash_create(1024);
  bbl->elist = list_create(LIST_SINGLE);
}

void
bbl_entry(Mloc m, const char *tok)
{
  static int entry_index = 0;
  if (verbose)
    fprintf(stderr, "bbl_entry: tok=%s\n", tok);
  Bblentry *ep = memo_new(bblentry_m);
  ep->ref = tok;
  ep->index = entry_index++;
  hash_add(bbl->ehash, (uccp)ep->ref, ep);
  ep->h4t_ref = bxh4t_ref(tok);
  if (strcmp(ep->ref, ep->h4t_ref))
    hash_add(bbl->ehash, (uccp)ep->h4t_ref, ep);
}
