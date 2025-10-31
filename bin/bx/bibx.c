#include <oraccsys.h>
#include "bx.h"
#include "bib.h"

Bx b;

extern Mloc biblloc;
extern int bibparse(void);
extern void bibset_debug(int);
extern void bib_wrapup_buffer(void);
int bib_field, bib_nesting;
const char *curr_bib, *curr_key, *field;

const char *fldnames[f_top];

void
bib_load(const char *bibfile)
{
  extern const char *curr_bib;
  extern FILE *bibin;
  curr_bib = bibfile;
  FILE *fp = fopen(bibfile, "r");
  if (!fp)
    {
      fprintf(stderr, "can't read .bib %s\n", bibfile);
      exit(1);
    }
  /*bibentry_m = memo_init(sizeof(Bibentry),256);*/
  bibin = fp;
  field = NULL;
  bib_field = bib_nesting = 0;
  biblineno = 1;
  bibparse();
  bibreset();
  fclose(fp);
}

int
main(int argc, char * const*argv)
{
  setlocale (LC_ALL, ORACC_LOCALE);
  options(argc, argv, "");
  bibset_debug(0);
  b.p = pool_init();
  mesg_init();
  b.bibs = list_create(LIST_SINGLE);
  b.m_bib = memo_init(sizeof(Bib), 32);
  b.m_bibentry = memo_init(sizeof(Bibentry), 1024);
  int i;
  for (i = optind; argv[i]; ++i)
    {
      Bib *bp = memo_new(b.m_bib);
      bp->file = argv[i];
      bp->entries = list_create(LIST_SINGLE);
      list_add(b.bibs, bp);
      curr_bibp = bp;
      bib_load(argv[i]);
    }
  bib_xml(b.bibs, stdout);
}

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    }
  return 0;
}

void help(void) {}
