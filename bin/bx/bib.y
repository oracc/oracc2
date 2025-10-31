%define api.prefix {bib}
%file-prefix"bib"
%locations
%define parse.error verbose
%expect 1
%{

#include <oraccsys.h>
#include <mesg.h>
#include "bx.h"
#include "bib.h"

extern int yylex(void);
extern void yyerror(const char *);

#define BIBLTYPE_IS_DECLARED 1
#define BIBLTYPE Mloc
#define yylineno biblineno
BIBLTYPE biblloc;

int bib_field, bib_nesting;
const char *curr_bib, *curr_key, *field;
Bx *curr_bp;
Bib *curr_bibp;
Bibentry *curr_ep;
Hash *bib_cites;

static struct bib_fld_tab *curr_bfp;

%}

%union { char *s; int i; }

%token <i> BIB_ERROR
%token <s> BIB_ENTRY BIB_KEY BIB_FIELD BIB_CHUNK BIB_STRING

%start entries

%%

entries: entry
      | entries entry
      ;

entry: 	BIB_ENTRY '{' { bib_key_next=1; }
	BIB_KEY   ',' { bib_entry_init(@1, $1+1, $4); }/*+1 to skip '@' on entry names*/
	fields    '}' { bib_entry_term(@5); }
      ;

fields: field ','
      | fields field
      | fields field ','
      ;

field: BIB_FIELD '=' '{' { bib_field_init(@1, $1); } content '}' { bib_field_term(@6); }
      ;

content:
        nothing
      | bit
      | content bit
      ;

bit:  BIB_CHUNK		{ (void)bib_content(@1, $1); } ;


nothing: /* empty */
      ;

%%

void
bib_init(Bx *bp)
{
  curr_bp = bp;
  bp->m_bib = memo_init(sizeof(Bib), 32);
  bp->m_bibentry = memo_init(sizeof(Bibentry), 1024);
}

void
bib_entry_init(Mloc m, const char *ent, const char *key)
{
  bib_key_next = 0;
  curr_ep = memo_new(curr_bp->m_bibentry);
  if (!bib_ent(ent, strlen(ent)))
    fprintf(stderr, "warning: unknown entry type %s\n", ent);
  curr_ep->type = ent;
  curr_ep->key = key;
  if (bib_cites && hash_find(bib_cites, (uccp)key))
    list_add(curr_bp->entries, curr_ep);
}

void
bib_entry_term(Mloc m)
{
  if (bib_field || bib_nesting)
    mesg_vwarning(curr_bib, biblineno, "wrongly nested {...} in entry %s", curr_key);
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
      char *t = (char*)pool_copy((uccp)s, curr_bp->p);
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
  const char *f = biblloc.file;
  int l = biblloc.line;
  
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
