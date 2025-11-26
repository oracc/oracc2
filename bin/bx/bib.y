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
extern const char *bx_bid_first(const char *ids);

#define BIBLTYPE_IS_DECLARED 1
#define BIBLTYPE Mloc
#define yylineno biblineno
BIBLTYPE biblloc;

int bib_entry, bib_field, bib_nesting;
const char *curr_bib, *curr_key, *field, *curr_field;
Bx *curr_bp;
Bib *curr_bibp;
Bibentry *curr_ep;
Hash *bib_cites;

static struct bib_fld_tab *curr_bfp;
static Bibfield anonymous = { "Anonymous", 0 };
static List *comments = NULL;
static int bib_entry_id(Bibentry *ep);
 
%}

%union { char *s; int i; }

%token <i> BIB_ERROR
%token <s> BIB_ENTRY BIB_KEY BIB_FIELD BIB_CHUNK BIB_STRING COMMENT

%start contents

%%

contents:
	   content
	 | contents content
	 ;

content:
	 comment
       | entry
       ;

entry: 	BIB_ENTRY '{' { bib_key_next=1; }
	BIB_KEY   ',' { bib_entry_init(@1, $1+1, $4); }/*+1 to skip '@' on entry names*/
	fields    '}' { bib_entry_term(@5); }
      ;

comment:
	  COMMENT     { bib_comment(@1, $1); }
	;

fields: field ','
      | fields field
      | fields field ','
      ;

field: BIB_FIELD '=' '{' { bib_field_init(@1, $1); } fcontent '}' { bib_field_term(@6); }
      ;

fcontent:
        nothing
      | bit
      | fcontent bit
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
  bp->m_bibfield = memo_init(sizeof(Bibentry), 1024);
  bvl_init();
}

void
bib_eof(void)
{
  curr_bibp->eof_comments = comments;
  comments = NULL;
}

void
bib_comment(Mloc m, char *cmt)
{
  if (!comments)
    comments = list_create(LIST_SINGLE);
  list_add(comments, (void*)cmt);
}

void
bib_entry_init(Mloc m, const char *ent, const char *key)
{
  bib_key_next = 0;
  curr_ep = memo_new(curr_bp->m_bibentry);
  curr_ep->bib = curr_bibp;
  curr_ep->comments = comments; comments = NULL;
  if (!bib_ent(ent, strlen(ent)))
    mesg_vwarning(curr_bib, biblineno, "warning: unknown entry type %s\n", ent);
  curr_ep->type = ent;
  curr_ep->bkey = key;
  if (!bib_cites || hash_find(bib_cites, (uccp)key))
    list_add(curr_bp->entries, curr_ep);
  bib_entry = 1;
}

void
bib_entry_term(Mloc m)
{
  if (bib_field || bib_nesting)
    mesg_vwarning(curr_bib, biblineno, "wrongly nested {...} in entry %s", curr_key);

  int i;

  if ((!curr_ep->fields[f_author] && !curr_ep->fields[f_editor])
      || (curr_ep->fields[f_author] && !curr_ep->fields[f_author]->data))
    curr_ep->fields[f_author] = &anonymous;

  for (i = 0; i < f_top; ++i)
    {
      if (curr_ep->fields[i] && bib_validators[i])
        {
	  m.line = curr_ep->fields[i]->line;	
          bib_validators[i](&m,curr_bp,i,curr_ep);
	}
    }

  if (!curr_ep->fields[f_author])
    {
      if (curr_ep->nenames == 1)
        curr_ep->allnames = curr_ep->enames[0]->nkey;
      else
        bnm_all_names(curr_ep, f_editor);
    }

  if (!curr_ep->fields[f_ids] || !bib_entry_id(curr_ep))
    mesg_vwarning(curr_bib, biblineno, "no ids field or no B-ID in ids\n");

  curr_ep = NULL;
  bib_entry = bib_field = 0;
}

static int
bib_entry_id(Bibentry *ep)
{
  const char *B = bx_bid_first(ep->fields[f_ids]->data);
  if (B)
    {
      ep->bid = (char*)pool_alloc(8, curr_bp->p);
      strncpy(ep->bid, B, 7);
      ep->bid[7] = '\0';
    }
  return 0;
}

void
bib_field_init(Mloc m, const char *f)
{
  bib_field = 1;
  struct bib_fld_tab *bfp = bib_fld(f, strlen(f));
  if (!bfp)
    fprintf(stderr, "warning: unknown field type %s\n", f);
  else
    {
      curr_bfp = bfp;
      curr_ep->fields[curr_bfp->t] = memo_new(curr_bp->m_bibfield);
      curr_field = curr_ep->fields[curr_bfp->t]->name = bfp->name;
      curr_ep->fields[curr_bfp->t]->line = biblineno;
    }
}

void
bib_field_term(Mloc m)
{
  bib_field = 0;
  curr_field = NULL;
  if (curr_bfp)
    curr_ep->fields[curr_bfp->t]->data = bib_content(m, NULL);
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
  
  if (curr_field)
    mesg_vwarning(f, l, "field %s error: %s", curr_field, e);
  else if (curr_ep)
    mesg_vwarning(f, l, "%s error: %s", curr_ep->type, e);
  else
    mesg_vwarning(f, l, "%s\n", e);
}
