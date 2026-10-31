%define api.prefix {gdl}
%file-prefix"gdl"
%locations
%define parse.error verbose

%{
#include <stdlib.h>
#include <stdio.h>
#include <mesg.h>
#include "gdllib.h"

extern int gdllex(void);
extern void yyerror(const char *);
extern const char *gdlfile;
extern int gdllineno, gdltrace;

#if 1
#define ytp tcurr
#else
static Tree *ytp;
#endif

static Node *ynp, *yrem, *ycp, *mnp;

#define GDLLTYPE_IS_DECLARED 1
#define GDLLTYPE Mloc
#define yylineno gdllineno
GDLLTYPE gdllloc;

%}

%union { char *text; int i; struct node *node; }

###gdltokens.y###

%nterm <text> 	field lexfld cmods

%nterm <node>   detgloss det gloss
		grapheme scgrapheme compound simplexg valuqual
		glosso glossc

%start gdl.line

%%

gdl.line: { ynp = gdl_new_word(tcurr); } lineparts
	;

lineparts:
		linepart
	|	lineparts linepart
		;

linepart:	cell
	| 	field
	|	segment
		;

cell:
		cellspec
		;

cellspec:
		'&'			       	    	{ gdl_cell(ytp,"1"); }
	| 	CELLSPAN			      	{ gdl_cell(ytp,$1); }
		;


/* Field delim is ',' and FTYPE is !yn or the like; technically, !yn
   does not need a preceding comma with this grammar.*/

field:
		',' 		       	    		{ ynp = gdl_field(ytp,"default"); }
	| 	FTYPE		       		    	{ ynp = gdl_field(ytp,$1); }
        | 	lexfld				    	{ ynp = gdl_field(ytp,$1); }
		;

lexfld:
		LF_AT		{ $$ = $1; }
	| 	LF_CARET	{ $$ = $1; }
	| 	LF_EQUALS	{ $$ = $1; }
	| 	LF_HASH		{ $$ = $1; }
	| 	LF_QUOTE	{ $$ = $1; }
	| 	LF_TILDE	{ $$ = $1; }
	| 	LF_VBAR		{ $$ = $1; }
		;

segment:
		comment
	| 	space
	| 	linebreak
	| 	transliteration
		;

comment:
		L_inl_dol TEXT R_inl_dol	        { gdl_nongraph(&@1, ytp, $2, "dollar"); }
	| 	L_inl_cmt TEXT R_inl_cmt	        { gdl_nongraph(&@1, ytp, $2, "comment"); }
		;

space:
	  SPACE						{ gdl_hc(0); ynp = gdl_new_word(ytp);
	    						  /*gdl_prop_kv(ynp, GP_ATTRIBUTE, PG_GDL_INFO,
	      						    "literal", gdllval.text); */ }
	| '\n'						{ gdl_line_wrapup(@1); }
	| END						{ gdl_balance_flush(&@1); }
	;

linebreak:	LINEBREAK	      			{ gdl_nongraph(&@1, ytp, "//", "linebreak"); }
	;

transliteration:
		delim
	| 	grapheme
	| 	normseg
	;

delim:
		'.' 				       	{ ynp = gdl_delim(ytp, "."); }
        | 	'-' 			       		{ ynp = gdl_delim(ytp, "-"); }
	| 	ENHYPHEN 	       	       		{ ynp = gdl_delim(ytp, "--"); }
	|	'/'					{ gdl_group(@1, '/'); }
	|	':'					{ gdl_group(@1, ':'); }
	|	'+'					{ gdl_group(@1, '+'); }
	;

normseg:	NORMSEG					{ ynp = gdl_normseg(&@1, ytp, gdllval.text); }
	;

detgloss:
		det
	|	gloss
	;

det:	  '{'	      					{ gdl_balance_state(&@1,'{',gdl_lex_state());
	      						  $$ = ynp = gdl_push_l(&@1,ytp,"g:d");
	  						  ps_on(gs_det_o);
	  						  rs_on(gs_det|gs_g_semd_i); }
	| DET_SEME    					{ gdl_balance_state(&@1,'{',gdl_lex_state());
	    						  $$ = ynp = gdl_push_l(&@1,ytp,"g:d");
	  						  ps_on(gs_det_o);
	  						  rs_on(gs_det|gs_g_semd_e); }
	| DET_PHON      	      			{ gdl_balance_state(&@1,'{',gdl_lex_state());
	    						  $$ = ynp = gdl_push_l(&@1,ytp,"g:d"); 
	  						  ps_on(gs_det_o);
	  						  rs_on(gs_det|gs_g_phond); }
	| '}' 	 		  			{ if (-1 != gdl_balance_state(&@1,'}',gdl_lex_state()))
		  					    gdl_det_props(ytp->curr);
		    					  if (gdl_post_det_gp_attach)
							    {
								tree_curr(gdl_post_det_gp_attach);
								gdl_post_det_gp_attach = NULL;
							    }
							  $$ = lgp = ytp->curr;
	      						  ynp = gdl_pop(ytp,"g:d");
	     						  /* set pst->det = SB_CL; lgp is last
							     node with g content or equivalent, i.e.,
						             where the closer state must be set */
	    						  /*gdl_update_state(lgp, gs_det_c);*/
	      						  bit_set(*lst, gs_det_c);
						          rs_no(gs_det|gs_g_semd_e|gs_g_semd_i|gs_g_phond);
	  				       	        }
	;

gloss:		glosso
	|	glossc
	;

graphemes:	grapheme
	|	graphemes grapheme
	;

grapheme:
		scgrapheme
	| 	valuqual
	|	detgloss
		;

scgrapheme:
		simplexg
	| 	compound
		;

maybegflags:
		mods
	| 	/*empty */
		;

simplexg:	s maybegflags	       	       	 { if (ynp->kids) gdl_mod_wrap(ynp, 1);
						   gvl_simplexg(ynp);
						   $$ = ynp; }
		;

s:
		GRAPHEME			{ ynp = gdl_graph(&@1, ytp, gdllval.text); }
	| 	LISTNUM				{ ynp = gdl_listnum(&@1, ytp, gdllval.text); }
	| 	NNUM				{ ynp = gdl_nnum(&@1, ytp, gdllval.text); }
	| 	NUMBER				{ ynp = gdl_number(&@1, ytp, gdllval.text); }
	| 	BARENUM				{ ynp = gdl_barenum(&@1, ytp, gdllval.text); }
	| 	ZERO				{ ynp = gdl_graph(&@1, ytp, gdllval.text); } /* 00 or 00~a etc */
	| 	PUNCT				{ ynp = gdl_punct(&@1, ytp, gdllval.text); }
	| 	ELLIPSIS			{ ynp = gdl_nongraph(&@1, ytp, gdllval.text, "ellipsis"); }
	|	IGNORE				{ ; }
	;

compound:
		c cmaybemodflags       		{ ycp->mloc = ynp->mloc = mloc_mloc(&@1);
	    			       		  gdl_modq_flush();
				       		  gvl_compound(ycp);
				       		  $$ = ycp;
	    			       		  ynp = gdl_pop(ytp,"g:c");
				       		  ynp = gdl_c_term(ytp); }
	;

c:
	C_O 						{ ycp = gdl_push_l(&@1,ytp,"g:c");
	    						  gdl_c_init(ycp); }
	cbits
	C_C 						{ lgp = ynp = ycp; lst = prop_state(ycp,0L); c_processing = 0; }
        ;

cbits:
	  cbit
        | cbits cbit
	;

cbit:
	s	       			       		{ /*ynp->mloc = mloc_mloc(&@1);*/
	  						  /*if (gdl_legacy) gdl_unlegacy(ynp);*/
							  gvl_simplexg(ynp); }
	| cdelim					{ }
	| CLP			       			{ gdl_balance_state(&@1,CLP,gdl_lex_state());
	    					  	  gdl_push_l(&@1,ytp,"g:gp"); }
	| CRP	     			    		{ if (-1 != gdl_balance_state(&@1,CRP,gdl_lex_state()))
	      					    	   gdl_pop(ytp,"g:gp"); }
	| QLP 						{ yrem=kids_rem_last(ytp);
	    						  gdl_push_l(&@1,ytp,"g:q");
							  kids_add_node(ytp,yrem);
							  gdl_remove_q_error(@1, yrem);
  							  gdl_incr_qin();
							}
	| QRP				      		{ gdl_decr_qin();
	  						  ynp->mloc = mloc_mloc(&@1);
	  						  gvl_valuqual(ytp->curr);
							  lgp = ytp->curr;
							  ynp = gdl_pop(ytp,"g:q");
							}
	| mod						{ gdl_modq_add(mnp); mnp = NULL; };
	;

cdelim:
		C_PERIOD	       			{ ynp = gdl_delim(ytp, "."); }
	| 	C_ABOVE					{ ynp = gdl_delim(ytp, "&"); }
	| 	C_COLON					{ ynp = gdl_delim(ytp, ":"); }
	| 	C_CROSSING     				{ ynp = gdl_delim(ytp, "%"); }
	| 	C_OPPOSING     				{ ynp = gdl_delim(ytp, "@"); }
	| 	C_PLUS					{ ynp = gdl_delim(ytp, "+"); }
	| 	C_TIMES					{ ynp = gdl_delim(ytp, "×"); }
	| 	C_3TIMES			       	{ gdl_prefix(ytp, (uccp)"3"); }
	| 	C_4TIMES		       		{ gdl_prefix(ytp, (uccp)"4"); }
	| 	C_CIRCLE	       			{ ynp = gdl_delim(ytp, "∘"); } /* for PC */
	;

cmaybemodflags:
		cmods
	| /* empty */
	;

cmods:
	  GMOD						{ mnp = gdl_mod_cmod(&@1, ynp, NULL, $1); }
	| cmods GMOD					{ (void)gdl_mod_cmod(&@1, ynp, mnp, $1); }
	;

valuqual:
	q	    				       	 { ynp->mloc = mloc_mloc(&@1);
	  						   gvl_valuqual(ytp->curr);
	  						   lgp = ynp = ytp->curr; }
	qmaybemodflags					 { gdl_mod_wrap_q(ynp);
	    						   $$ = lgp;
							   ynp = gdl_pop(ytp,"g:q"); }
        ;

q:
	grapheme
	QLP 						{ yrem=kids_rem_last(ytp);
	    						  gdl_push_l(&@1,ytp,"g:q");
							  kids_add_node(ytp,yrem);
							  gdl_incr_qin(); }
	graphemes 	 	       			{ gdl_remove_q_error(@1, yrem); }		
	QRP 						{ gdl_decr_qin(); }
	;

qmaybemodflags:
		mods
	| /* empty */
	;

mods:
	  mod
	| mods mod
        ;

mod:
	  GMOD						{ ynp->mloc = mloc_mloc(&@1);
	  						  mnp = gdl_mod(ytp, gdllval.text); }
	;

glosso:
		L_cur_par	{ ynp = gdl_gloss_o(&@1, ytp, gdllval.text, e_L_cur_par); }
	| 	L_dbl_cur	{ ynp = gdl_gloss_o(&@1, ytp, gdllval.text, e_L_dbl_cur); }
	| 	L_ang_par_s	{ ynp = gdl_gloss_o(&@1, ytp, gdllval.text, e_L_ang_par); }
	;

glossc:
		R_cur_par      	{ ynp = gdl_gloss_c(&@1, ytp, gdllval.text, e_R_cur_par); }
	| 	R_dbl_cur      	{ ynp = gdl_gloss_c(&@1, ytp, gdllval.text, e_R_dbl_cur); }
	| 	R_ang_par_s    	{ ynp = gdl_gloss_c(&@1, ytp, gdllval.text, e_R_ang_par); }
	;

%%

#if 0
	/* We need this to give gdl_meta access to the tree when there is no lgp set */
Tree *
gdl_get_tree(void)
{
  return ytp;
}

void
gdl_set_tree(Tree *tp)
{
  ytp = tp;
  ynp = gdl_new_word(ytp);
}
#endif

void
gdlerror(const char *e)
{
  mesg_vwarning(gdlfile, gdllineno, "gdl: %s\n", e);
}
