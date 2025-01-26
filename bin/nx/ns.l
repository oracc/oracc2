%option prefix="ns"
%option never-interactive noyywrap yylineno nounput

%{
#include <ctype.h>
#include <mesg.h>
#include <pool.h>

extern Pool *nspool;
#define nssave(s) pool_copy((const unsigned char*)(s),nspool);
#define nsave  nslval.text=nssave(nstext)

#define NSLTYPE Mloc
#include "ns.tab.h"
extern Mloc nslloc;
const char *nsfile = "<stdin>";
const char *currnsfile = NULL;
int nsflextrace = 0;

%}

S		\xe2\xb8[^\xa2-\xa5]
G		([a-zA-Z\x80-\xc2\xc4-\xe1\xe3-\xff]|{S}|\xc3[^\x97]|\xe2[^\xb8])+
M		([@~\\]([a-wyz]+[0-9]*|90|180|270))
NUM		([0-9]+|n)(\.[0-9]+)?
FRAC		[0-9n]\/[0-9]

%%

%[^ \t]+				{ nsave; return NS_NAME; }

[0-9.]+(cm2?|ha|kg|mm|m2?|g|l|m|u)     	{ nsave; return NS_CONV; }

{NUM}\(({G}{M}*|N[0-9]+)\)	       	{ nsave; return NU_GVAL; }

{FRAC}\({G}{M}*\)			{ nsave; return NU_FRAC; }

"$"(su|sa|sd|sl|sw)			{ nsave; return NE_BOO; }

"$"(mt|mp|mg|ml|c|f|j|l|m|n|p|r|s|t)	{ nsave; return NE_KEY; }

\"[^\"\n\t]*\"				{ nsave; return NE_VAL; }

{NUM}|{FRAC}	       			{ nsave; return NU_NUM; }

(A|D)(\@[ctv])*/[n0-9]*               	{ nsave; return NU_AXIS; }

\(?{G}{M}*("."{G}{M}*)*\)?  		{ nsave; return NU_UNIT; }

\{{G}{M}*("."{G}{M}*)*\}?  		{ nsave; return NU_DET; }

[|"*=>.:/\\]			       	{ return *nstext; }

[ \t\n]+				{ }

%%

/* FUNCTIONS HERE */
