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
extern int nslineno, nstrace, nsb_altflag;

#define NSLTYPE_IS_DECLARED 1
#define NSLTYPE Mloc
#define yylineno nslineno
NSLTYPE nsylloc;

uchar one[2] = {'1','\0'};
 
%}

%union { unsigned char *text; int i; }

%token <text> 	NS_NAME NS_BASE NS_CONV NS_STOP
		NE_BOO NE_KEY NE_VAL
		NU_AXIS NU_FRAC NU_GVAL NU_NUM NU_UNIT NU_DET
		END

%nterm <text> 	ns_base ns_conv nu_xunit nu_gval

%start ns

%%

ns: 	  ns_sys
	| ns ns_sys
	;

ns_sys:	  ns_name ns_equiv ns_nu ns_det ns_stop
	| ns_name ns_equiv ns_nes ns_nu ns_det ns_stop
	| ns_name ns_equiv ns_nu ns_det ns_nis ns_stop
	| ns_name ns_equiv ns_nes ns_nu ns_det ns_nis ns_stop
	;

ns_name: NS_NAME				{ nsb_sys($1); }
	;

ns_equiv: ns_base ns_conv			{ nsb_equiv($1,$2); }
	| /* empty */
	;

ns_base:  NU_UNIT
	;

ns_conv:  NS_CONV
	;

ns_nes:	  ns_ne
	| ns_nes ns_ne
	;

ns_ne: 	  NE_BOO				{ nsb_env($1, one); }
	| NE_KEY '=' NE_VAL			{ nsb_env($1, $3); }
	;

ns_nu:    nu_multunit
	| ns_nu '=' nu_multunit
	| ns_nu '|' { nsb_altflag=1; } nu_multunit
	;

nu_multunit: NU_NUM nu_xunit   			{ nsb_step(NULL,$1,$2); }
	| NU_AXIS NU_NUM nu_xunit   		{ nsb_step($1,$2,$3); }
	;

nu_gval:  NU_FRAC | NU_GVAL ;

nu_xunit: '*' NU_FRAC				{ $$ = $2; }
	| '*' NU_UNIT		       		{ $$ = $2; }
	| '*' NU_GVAL		       		{ $$ = $2; }
	;

ns_det:    NU_DET				{ nsb_det($1); }
	| /* empty */
	;

ns_nis:   ns_ni
	| ns_nis ns_ni
	;

ns_ni:    nu_gval '>' NU_NUM nu_xunit		{ nsb_inst_g($1,$3,$4, NS_INST_DATA); }
	/*| NI_AXIS '+' NU_UNIT '>' NU_UNIT	{ nsb_inst_u($1,$3,$5, NS_INST_DATA); }*/
	;

ns_stop: '.'					{ nsb_wrapup(); }
	;

%%

void
nserror(const char *e)
{
  mesg_vwarning(currnsfile, nslineno, "%s: %s\n", nstext, e);
}
