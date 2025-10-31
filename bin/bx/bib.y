%define api.prefix {bib}
%file-prefix"bib"
%locations
%define parse.error verbose
%expect 1
%{

#include <mesg.h>
#include "bib.h"

extern int yylex(void);
extern void yyerror(const char *);

#define BIBLTYPE_IS_DECLARED 1
#define BIBLTYPE Mloc
#define yylineno biblineno
BIBLTYPE biblloc;

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

