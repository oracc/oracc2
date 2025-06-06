%define api.prefix {asl}
%file-prefix"asl"
%locations
%define parse.error detailed
%define parse.lac full

%{
#include <stdio.h>
#include <string.h>
#include <mesg.h>
#include "asl.h"
extern int yylex(void);
extern void yyerror(const char *);
extern const char *asltext, *curraslfile;
extern int asllineno, asltrace;
extern int gdl_unicode;

#define ASLLTYPE_IS_DECLARED 1
#define ASLLTYPE Mloc
#define yylineno asllineno
ASLLTYPE asllloc;

int asl_raw_tokens = 0;
int asl_literal_flag = 0;
int minus_flag = 0;
 
%}

%union { char *text; int i; }

%token	<text>  TOK TRANS TAB EOL PAR BAD LINE SIGLUM
		SIGN PCUN XSUX FAKE AKA COMP PNAME FORM NOFORM
		GNAME GVALUE GVALUEX GOESTO
		GBAD ATF LANG V LIST LISTNUM OTFEAT LISTIMAGE
		INOTE LIT NOTE REF TEXT END EBAD
		ESIGN EPCUN EXSUX EFORM
		UAGE USEQ UTF8 UMAP UNAME UNOTE UPUA
		SIGNLIST PROJECT DOMAIN LISTDEF LISTNAME SREF
		SYSDEF SYSNAME SYS SYSGOESTO SYSTEXT
		SMAP LITERAL IMAGES MERGE OID
		LINKDEF LINK
		LIGA LIGATURE LIGAFONT LIGAUCUN
		SCRIPTDEF SCRIPT UTOKEN
		GLYF GLYF_NAM GLYF_OID GLYF_UNI GLYF_HEX GLYF_VAR GLYF_OTF

%nterm  <text>  anynote atftoken atftokens lang longtext token uniimg utoken utokens

%start fields

%%

fields: blank
	| field
	| fields blank
	| fields field
	;

blank:  EOL
       	| PAR
	;

field: 	  line EOL
	| line PAR	{ if (asltrace) fprintf(stderr, "PAR\n"); }
	| error EOL	{ yyerrok; }
	;

line:	  atcmd		{ if (asltrace) fprintf(stderr, "atcmd/EOL: %s\n", asllval.text); }
	;

longtext:
	  TEXT		{ $$ = longtext(curr_asl, $1, NULL); /* fprintf(stderr,"longtext TEXT %s\n",$1); */ }
        | longtext TAB	{ $$ = longtext(curr_asl, $1, $2);   /* fprintf(stderr,"longtext %s\n\t+ TAB %s\n",$1,$2);*/ }
	;

atcmd:
	  atsignlist
	| atproject
	| atdomain
	| atlinkdef
	| atlistdef
	| atscriptdef
	| atsysdef
	| atimages
	| atsign
        | atsref
        | atoid
        | ataka
        | atsmap
        | atcomp
        | atpname
	| atfake
	| atlist
	| atv
	| atliga
	| atlink
	| atsys
	| atscript
	| atmerge
	| atform
	| atglyf
	| atmeta
	| atunicode
	| atend
        ;

atsignlist:
	  SIGNLIST TEXT 		{ asl_bld_signlist(&@1, (uccp)$2, 0); }
	;

atproject:
	  PROJECT TEXT 			{ asl_bld_signlist(&@1, (uccp)$2, 1); }
	;

atdomain:
	  DOMAIN TEXT 			{ asl_bld_signlist(&@1, (uccp)$2, 2); }
	;

atlinkdef:
	  LINKDEF SYSNAME SYSTEXT     	{ asl_bld_linkdef(&@1, curr_asl, (ccp)$2, (ccp)$3); }
	;

atlistdef:
	  LISTDEF LISTNAME longtext 	{ asl_bld_listdef(&@1, curr_asl, (ccp)$2, (ccp)longtext(NULL,NULL,NULL)); }
	;

atsysdef:
	  SYSDEF SYSNAME	    	{ asl_bld_sysdef(&@1, curr_asl, (ccp)$2, NULL); }
	| SYSDEF SYSNAME SYSTEXT    	{ asl_bld_sysdef(&@1, curr_asl, (ccp)$2, (ccp)$3); }
	;

atscriptdef:
	  SCRIPTDEF TEXT		{ asl_bld_scriptdef(&@1, curr_asl, (char*)$2); }
	;

atimages:
	  IMAGES TEXT 		    	{ asl_bld_images(&@1, curr_asl, (uccp)$2); }
	;

atmerge:
	  MERGE TEXT 		    	{ asl_bld_merge(&@1, curr_asl, (uccp)$2); }
	;

atscript:
	  SCRIPT TEXT			{ asl_bld_script(&@1, curr_asl, (char*)$2); }
	;

atoid:
	  OID TEXT 		    	{ asl_bld_oid(&@1, curr_asl, (uccp)$2); }
	;

atsign:
	  SIGN token	 { asl_bld_sign(&@1, curr_asl, (uccp)$2, minus_flag); }
	| PCUN token	 { asl_bld_pcun(&@1, curr_asl, (uccp)$2, minus_flag); }
	| XSUX token	 { asl_bld_xsux(&@1, curr_asl, (uccp)$2, minus_flag); }
	| SIGN GBAD 	 /* not generated by asl.l yet */
	;

atfake:   FAKE 		{ if (curr_asl->curr_sign && !curr_asl->curr_form) { curr_asl->curr_sign->fake = 1; }
	    		  else { mesg_vwarning(curraslfile, asllineno, "asl: misplaced @fake line\n");  } }
	;

ataka:
	  AKA token 	 { asl_bld_aka(&@1, curr_asl, (uccp)$2); }
	;

atsmap:
	  SMAP token 	 { asl_bld_smap(&@1, curr_asl, (uccp)$2); }
	;

atcomp:
	  COMP token	 { (void)asl_bld_tle(&@1, curr_asl, (uccp)$2, NULL, sx_tle_componly); }
	;

atpname:
	  PNAME token 	 { asl_bld_pname(&@1, curr_asl, (uccp)$2); }
	;

atlink:
	  LINK SYSNAME SYSTEXT	{ asl_bld_link(&@1, curr_asl, (ccp)$2, (uccp)$3, NULL); }
	;

utoken:   UTOKEN		
	;

utokens:  utoken		{ $$ = longtext(curr_asl, $1, NULL); }
	| utokens utoken	{ $$ = longtext(curr_asl, $1, $2); }
	;

uniimg:   utokens
	| LISTIMAGE			{ $$ = $1; }
	;

atlist:
	  LIST LISTNUM
		{ asl_bld_list(&@1, curr_asl, (ccp)$1, (uccp)$2, minus_flag, NULL, NULL, NULL, NULL); }
	| LIST LISTNUM OTFEAT
		{ asl_bld_list(&@1, curr_asl, (ccp)$1, (uccp)$2, minus_flag, (uccp)$3, NULL, NULL, NULL); }
	| LIST LISTNUM LISTIMAGE
		{ asl_bld_list(&@1, curr_asl, (ccp)$1, (uccp)$2, minus_flag, NULL, NULL, (uccp)$3, NULL); }
	| LIST LISTNUM atftoken uniimg atftokens
		{ asl_bld_list(&@1, curr_asl, (ccp)$1, (uccp)$2, minus_flag, NULL, (uccp)$3, (uccp)$4, (uccp)longtext(NULL,NULL,NULL)); }
	| LIST LISTNUM atftoken uniimg atftokens OTFEAT
		{ asl_bld_list(&@1, curr_asl, (ccp)$1, (uccp)$2, minus_flag, (uccp)$6, (uccp)$3, (uccp)$4, (uccp)longtext(NULL,NULL,NULL)); }
	;

atliga:
          LIGA 	LIGATURE LIGAFONT LIGAUCUN { asl_bld_liga(&@1, curr_asl, (uccp)$2, (uccp)$3, (uccp)$4); }
	;

atglyf:
	  GLYF GLYF_NAM GLYF_UNI GLYF_HEX GLYF_OID GLYF_VAR
	  { asl_bld_glyf(&@1, curr_asl, (ccp)$2, (uccp)$3, (ccp)$4, (ccp)$5, (ccp)$6, NULL); }
	| GLYF GLYF_NAM GLYF_UNI GLYF_HEX GLYF_OID GLYF_VAR GLYF_OTF
	{ asl_bld_glyf(&@1, curr_asl, (ccp)$2, (uccp)$3, (ccp)$4, (ccp)$5, (ccp)$6, (ccp)$7); }
	;

/*
atlref:
	  LREF LISTNUM 	 { asl_bld_tle(&@1, curr_asl, (uccp)$2, NULL, sx_tle_lref); }
	| LREF LISTNUM GOESTO atftoken
			 { asl_bld_tle(&@1, curr_asl, (uccp)$2, (uccp)$4, sx_tle_lref); }
	;
*/

atsref:
	SREF atftoken GOESTO atftokens { (void)asl_bld_tle(&@1, curr_asl, (uccp)$2, (uccp)longtext(NULL,NULL,NULL), sx_tle_sref); }
	;

atform:
	  FORM token 		{ asl_bld_form(&@1, curr_asl, (uccp)$2, minus_flag); }
	| FORM GBAD
	;

atv:
	  V lang ATF     	{ asl_bld_value(&@1, curr_asl, (uccp)$3, (ccp)$2, 1, minus_flag); }
	| V lang GVALUE  	{ asl_bld_value(&@1, curr_asl, (uccp)$3, (ccp)$2, 0, minus_flag); }
	| V lang GBAD 
	;

atsys:
	  SYS SYSNAME SYSTEXT	{ asl_bld_sys(&@1, curr_asl, (ccp)$2, (uccp)$3, NULL); }
	| SYS SYSNAME SYSGOESTO SYSTEXT {
	    			  asl_bld_sys(&@1, curr_asl, (ccp)$2, (uccp)$3, (uccp)$4); }
        ;

atftokens:
	  atftoken		{ $$ = longtext_sep(curr_asl, $1, NULL, " "); }
	| atftokens atftoken	{ $$ = longtext_sep(curr_asl, $1, $2, " "); }
	;

atftoken:
	  ATF
	| token
	;

/*
tokens:
	  token		{ $$ = longtext(curr_asl, $1, NULL); }
	| tokens token  { $$ = longtext(curr_asl, $1, $2); }
	;
 */

token:
	  GNAME
	| GVALUE
	| LISTNUM
	| LITERAL		{ asl_literal_flag = 1; }
	;

/* Possibly add EGROUP ELETTER ESECTION here; on ESIGN we should NULL out sl->curr_sign/form/inst */
atend:
	  END ESIGN	    	{ asl_bld_end_sign(&@1, curr_asl, sx_tle_sign); }
	| END EPCUN	    	{ asl_bld_end_sign(&@1, curr_asl, sx_tle_pcun); }
	| END EXSUX	    	{ asl_bld_end_sign(&@1, curr_asl, sx_tle_xsux); }
	| EFORM			{ asl_bld_end_form(&@1, curr_asl); }
	| END EBAD
	;

/* There is no atuhex because we use @list U+ */
atunicode:
	  atuage
	| atuseq
	| atutf8
	| atumap
	| atupua
	| atuname
	| atunote
	;

atuage:
	  UAGE TEXT		{ asl_bld_urev(&@1, curr_asl, (uccp)$2); }
	;

atuseq:
	  USEQ TEXT		{ asl_bld_useq(&@1, curr_asl, (uccp)$2); }
	;

atutf8:
	  UTF8 TEXT		{ asl_bld_utf8(&@1, curr_asl, (uccp)$2); }
	;

atumap:
	  UMAP TEXT		{ asl_bld_umap(&@1, curr_asl, (uccp)$2); }
	;

atupua:
	  UPUA TEXT		{ asl_bld_upua(&@1, curr_asl, (uccp)$2); }
	;

atuname:
	  UNAME	TEXT		{ asl_bld_uname(&@1, curr_asl, (uccp)$2); }
        ;

atunote:
	  UNOTE longtext		{ asl_bld_unote(&@1, curr_asl, (uccp)longtext(NULL,NULL,NULL)); }
	;

atmeta:
	  anynote longtext	{ asl_bld_note(&@1, curr_asl, (ccp)$1, (ccp)$2); }
        ;

anynote:
	  INOTE			{ $$ = "inote"; }
	| LIT			{ $$ = "lit"; }
	| NOTE			{ $$ = "note"; }
	| REF			{ $$ = "ref"; }
	| SYS			{ $$ = "sys"; }
        ;

lang:
	  LANG
	| /* empty */ { $$ = NULL; }
	;

%%

void
aslerror(const char *e)
{
  mesg_vwarning(curraslfile, asllineno, "asl: %s\n", e);
}
