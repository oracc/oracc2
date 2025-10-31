#include <oraccsys.h>
#include "bib.h"

extern Mloc biblloc;
extern int bibparse(void);
extern void bibset_debug(int);
extern void bib_wrapup_buffer(void);
int bib_field, bib_nesting;
const char *curr_bib, *curr_key, *field;
static Bib *curr_bp;
static Bibentry *curr_ep;
static struct bib_fld_tab *curr_bfp;

List *bibs; /* each file is kept in a Bib in this list */
Memo *m_bib;
Memo *m_bibentry;
Pool *p;

const char *fldnames[f_top];

static void
bib_nesting_check(void)
{
  if (bib_field || bib_nesting)
    mesg_vwarning(curr_bib, biblineno, "wrongly nested {...} in entry %s", curr_key);
}

void
bib_entry_init(Mloc m, const char *ent, const char *key)
{
  bib_key_next = 0;
  curr_ep = memo_new(m_bibentry);
  if (!bib_ent(ent, strlen(ent)))
    fprintf(stderr, "warning: unknown entry type %s\n", ent);
  curr_ep->type = ent;
  curr_ep->key = key;
  list_add(curr_bp->entries, curr_ep);
}

void
bib_entry_term(Mloc m)
{
  bib_nesting_check();
}

void
bib_field_init(Mloc m, const char *f)
{
  bib_field = 1;
  struct bib_fld_tab *bfp = bib_fld(f, strlen(f));
  if (!bfp)
    fprintf(stderr, "warning: unknown field type %s\n", f);
  else
    curr_bfp = bfp;
}

void
bib_field_term(Mloc m)
{
  bib_field = 0;
  if (curr_bfp)
    curr_ep->fields[curr_bfp->t] = bib_content(m, NULL);
}

char *
bib_content(Mloc m, const char *bit)
{
  static char *s = NULL;
  static int used = 0;
  static int alloced = 0;

  if (bit == NULL)
    {
      char *t = (char*)pool_copy((uccp)s, p);
      free(s);
      s = NULL;
      used = alloced = 0;
      return t;
    }
  else
    {
      int len = strlen(bit);
      while (alloced < (used + len + 1))
	alloced += 1024;
      s = realloc(s, alloced);
      strcpy(s+used, bit);
      used += strlen(bit);
      return s;
    }
}

void
biberror(const char *e)
{
#if 1
  const char *f = biblloc.file;
  int l = biblloc.line;
#else
  const char *f = l_biblloc_file();
  int l = l_biblloc_line();
#endif
  
  if (field)
    {
      if ('@' == *field)
	fprintf(stderr, "%s:%d: %s error: %s\n", f, l, field, e);
      else
	fprintf(stderr, "%s:%d: field %s error: %s\n", f, l, field, e);
    }
  else
    fprintf(stderr, "%s:%d: %s\n", f, l, e);
}

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
  p = pool_init();
  mesg_init();
  bibs = list_create(LIST_SINGLE);
  m_bib = memo_init(sizeof(Bib), 32);
  m_bibentry = memo_init(sizeof(Bibentry), 1024);
  int i;
  for (i = optind; argv[i]; ++i)
    {
      Bib *bp = memo_new(m_bib);
      bp->file = argv[i];
      bp->entries = list_create(LIST_SINGLE);
      list_add(bibs, bp);
      curr_bp = bp;
      bib_load(argv[i]);
    }
  bib_xml(bibs);
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
