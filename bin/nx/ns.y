%define api.prefix {ns}
%file-prefix"ns"
%locations
%define parse.error verbose

%{
#include <stdlib.h>
#include <stdio.h>
#include <mesg.h>
#include "ns.h"

extern int nslex(void);
extern void yyerror(const char *);
extern const char *nstext, *currnsfile;
extern int nslineno, nstrace;

#define NSLTYPE_IS_DECLARED 1
#define NSLTYPE Mloc
#define yylineno nslineno
NSLTYPE nsylloc;

%}

%union { unsigned char *text; int i; }

%token <text> 	NS_NAME NS_BASE NS_CONV NS_STOP
		NE_BOO NE_KEY NE_VAL
		NI_AXIS
       		NU_GVAL NU_NUM NU_UNIT
		END

%nterm <text> 	ns_base ns_conv

%start ns

%%

ns: 	  ns_sys
	| ns ns_sys
	;

ns_sys:	  ns_name ns_equiv ns_nu ns_stop
	| ns_name ns_equiv ns_nes ns_nu ns_stop
	| ns_name ns_equiv ns_nu ns_nis ns_stop
	| ns_name ns_equiv ns_nes ns_nu ns_nis ns_stop
	;

ns_name: NS_NAME			{ nsb_sys($1); }
	;

ns_equiv: ns_base ns_conv		{ nsb_equiv($1,$2); }
	| /* empty */
	;

ns_base:  NU_UNIT
	;

ns_conv:  NS_CONV
	;

ns_nes:	  ns_ne
	| ns_nes ns_ne
	;

ns_ne: 	  NE_BOO
	| NE_KEY '=' NE_VAL
	;


ns_nu:    nu_multunit
	| ns_nu '=' nu_multunit
	;

nu_multunit: NU_NUM '*' NU_UNIT		{ nsb_step($1,$3); }
	;

ns_nis:   ns_ni
	| ns_nis ns_ni
	;

ns_ni:    NU_GVAL '>' NU_NUM '*' NU_UNIT
	| NI_AXIS '+' NU_UNIT '>' NU_UNIT
	;

ns_stop: '.'
	;

%%

void
nserror(const char *e)
{
  mesg_vwarning(currnsfile, nslineno, "nx: %s\n", e);
}
