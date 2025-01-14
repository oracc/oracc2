#ifndef SIGNLIST_H_
#define SIGNLIST_H_

#include <hash.h>
#include <list.h>
#include <roco.h>
#include <pool.h>
#include <tree.h>
#include <mesg.h>
#include <gsort.h>

struct sl_inst;
struct sl_listdef;

enum sx_tle
  {
    sx_tle_none ,
    sx_tle_componly ,
    sx_tle_fcomponly ,
    sx_tle_formproxy ,
    sx_tle_lref ,
    sx_tle_sign ,
    sx_tle_pcun ,
    sx_tle_xsux ,
    sx_tle_sref
 };

#define IMH_FILE 0
#define IMH_FONT 1
struct sx_iheader
{
  Roco *r;
  int order;
  const char *id;
  const char *label;
  const char *title;
  const char *page;
  const char *proj;
  const char *list;
  const char *path;
  const char *thumb;
  const char *period;
  int type; /* 0 = file; 1 = FONT */
  const char *css;
  const char *mag;
  const char *data;
  int sparse;
  Mloc mloc;
};

struct sl_signlist
{
  const char *signlist; /* The signlist name may vary from the host project, e.g., pcsl in pcsl */
  const char *project;  /* The project member is the host project for the signlist, e.g., pcsl */
  const char *domain; 	/* Default "sl"; for proto-cuneiform "pc";
			   other future values are possible, e.g.,
			   "pe" (Proto-Elamite) */
  struct sl_inst *notes;/* Allow inotes etc., after @signlist */
  Hash *linkdefs; 	/* Hash of signlist names; value is struct sl_linkdef */
  Hash *listdefs; 	/* Hash of signlist names; value is struct sl_listdef */
  Hash *sysdefs; 	/* Hash of system names; value is struct sl_sysdef */
  Hash *hkeys;		/* Hash of SIGN.FORM.VALUE keys point to
			   sl_inst for sign/form/v; unlike tokens
			   these use symbolic names, e.g., A.. A..a
			   A.LAK796.a */
  Hash *htoken; 	/* Every token that is a sign/form/list/value/base
			   as a struct sl_token * */
  Hash *hsentry; 	/* All the @sign/@sign- entries in the signlist */
  Hash *hfentry; 	/* All @form/@form- entries in signlist; host for sl_form* */
  Hash *haka;		/* Hash of @aka entries pointing to sl_sign or sl_form */
  Hash *hventry; 	/* All @v/@v- entries in signlist; host for sl_value* */
  Hash *hlentry; 	/* All @list/@list- entries in signlist; host for sl_list* */
  Hash *hsignvvalid; 	/* All @v which belong to a @sign,
			   not those belonging to @form; no @v- */
  Hash *homophones;	/* Hash of value-bases each with list of
			   sl_split_value* that reduce to that base;
			   x-values include the 'ₓ' in their base */
  Hash *homophone_ids;  /* IDs for homophones */
  Hash *hcompoundnew;	/* Signs reported in sx_compound_new_sign to prevent multiple error messages */
  Hash *values_by_oid;
  Hash *oidindexes;
  Hash *hletters;
  Hash *h_idata;	/* Hash used by sx_idata; adding graphemic data */
  Hash *h_mdata;	/* Hash used by sx_idata; adding graphemic data for mergers under the merge head */
  Hash *h_fdata;	/* Hash used by sx_idata; adding graphemic data for sign+forms */
  Hash *h_ldata;	/* Hash used by sx_ldata; adding lemma data */
  Hash *h_kdata;	/* Hash used by sx_kdata; subsetting by key */
  List *kdata_cpds;	/* List of compounds referenced in subset */
  Hash *h_merge;	/* Tracking hash for @merge contents when processing the results of subsetting */
  Hash *h_merges;	/* Hashes for managing the merge data while creating a subsl */
  Hash *h_merges_cand;
  Hash *h_merges_seen;
  Hash *h_scripts;	/* Hash for @scriptdef; value is an sl_script* */
  Hash *h_compoids;	/* in sx_unicode we track expansion of sipad to PA.LU as oid sequences */
  struct sl_token **tokens; /* sorted htoken */
  struct sl_sign  **signs;  /* sorted hsentry */
  int nsigns;
  struct sl_form  **forms;  /* sorted hfentry */
  int nforms;
  struct sl_value **values;  /* sorted hventry */
  int nvalues;
  struct sl_list  **lists;  /* sorted hlentry */
  int nlists;
  struct sl_letter *letters;
  int nletters;
  int ninsts;
  struct sl_number *numbers; /* sorted number tokens */
  int nnumbers;
  struct sl_numset *numsets; /* numbers grouped by set name */
  int nnumsets;
  struct sl_split_value *splitv;
  struct sl_sign *curr_sign;
  struct sl_inst *curr_form;
  struct sl_inst *curr_value;
  struct sl_inst *curr_inst; /* used to attach meta to correct tag */
  int curr_invalid; 		/* 1=@sign- 2=@form- 3=@v- */
  List *compounds;
  List *componly;		/* @compoundonly sl_inst *; can be sign-level or form-level */
  List *linklists; 		/* list of the lists of @link that occur in sign or
		     		   form, so we can generate link reports easily */
  List *syslists; 		/* list of the lists of @sys that occur in sign or
		     		   form, so we can generate system tables easily */
  List *images; 		/* list of names of image manifests as char * */
  Hash *hnums;			/* Hash of token pointers where gdl is g:n */
  struct sl_config *config;	/* settings from 00lib/config.xml */
  struct sx_iheader *iheaders;	/* array of header data read from @cmds in image manifests */
  Roco *iarray;			/* images data read into a Roco array */  
  Hash *oid2ucode;
  Memo *m_tokens;
  Memo *m_letters;
  Memo *m_groups;
  Memo *m_signs;
  Memo *m_signs_p;
  Memo *m_forms;
  Memo *m_lists;
  Memo *m_values;
  Memo *m_insts;
  Memo *m_insts_p;
  Memo *m_lv_data;
  Memo *m_split_v;
  Memo *m_compounds;
  Memo *m_digests;
  Memo *m_parents;
  Memo *m_notes;
  Memo *m_memostr;
  Memo *m_syss;
  Memo *m_links;
  Memo *m_ligas;
  Memo *m_idata;
  Memo *m_scriptdefs;
  Memo *m_scriptdata;
  Pool *p;
  Mloc mloc;
  Mloc eloc;
};

struct sl_token
{
  const unsigned char *t;	/* sign/form/value/base/list name token */
  Node *gdl;			/* token as GDL */
  GS_head *gsh;			/* GDL gsort data */
  const char *gsig;		/* return value from gdlsig run on
				   parsed GDL; NULL if the token was a
				   literal l*/
  const char *deep;		/* deep sig from gdlsig */
  struct sl_inst *oid_ip;	/* instance to use for retrieving OID */
  int s;			/* sort code for token */
  int priority;			/* for registering nums */
};

struct sl_liga
{
  unsigned const char *n; /* name for ligature, e.g., AN+EN */
  unsigned const char *f; /* font sequence, e.g., u1202D_u200D_u12097 */
  unsigned const char *u; /* cuneiform in UTF-8 */
};

/* Parents of values */
struct sl_parents
{
  List *signs; /* list of insts; plural because xvalues can legally have multiple parents */
  List *forms; /* list of insts */
  /* Sorted array of OIDs for easy printing; if qvsign and qvform it's
     all OIDs; if just qvform it's only form OIDs */
  const char **qvoids;
};

struct sl_split_value
{
  const unsigned char *v;
  const unsigned char *b;
  int i;
  const char *oid;
  const char *ref;
  int s;
};

/* Note information is stored in a single list so that within a note
   group the order is preserved in identity output; note groups may
   move within a sign block because they are only attached to items
   that are pointed to by sl->curr_inst */
struct sl_note
{
  const char *tag;
  const char *txt;
};

struct sl_unicode
{
  const unsigned char *utf8; 	/* the character(s) in UTF-8 */
  const char *uhex;		/* the U+HHHHH code for an encoded character */
  const char *useq;		/* for characters not encoded as singletons,
				   a sequence of hex values to render the sign name */
  const char *upua; 		/* for unencoded character(s), the hex code for a PUA
				   codepoint in the form xXXXXX */
  const char *umap; 		/* map of current @sign or @form to another @sign
				   or @form for obtaining Unicode info */
  const char *urev; 		/* the Unicode revision */
  const char *uname;		/* the Unicode name */
  List *unotes;			/* Unicode-related notes on the character and possibly related characters */
  int printed;			/* used if Unicode info should only be output once in a format */
};

#if 0
  const unsigned char *ivs;	/* the characters with IVS if any;
				   this normally points to 'utf8'; IVS
				   sequences are pairs--this version
				   has the pairs */
  const unsigned char *ren;	/* the characters with IVS rendering
				   if any; this normally points to
				   'utf8'; this version has IVS pairs
				   replaced with the rendering value
				   in the .oiv file; in a future
				   version this may include <img> tags
				   inlined to indicate that the IVS
				   rendering is implemented as an
				   image */
#endif

/* List and value data for @form insts */
struct sl_lv_data
{
  Hash *hlentry; 	/* All @list entries */
  /*Hash *hlvalid;*/	/* @list entries except @list- ; NULL unless there is an @list- */
  Hash *hventry;	/* All @v entries */
  Hash *hvbases;	/* All @v bases; used only for checking duplicates like a₂ and a₃ within a @form */
  /*Hash *hvvalid;*/	/* @v entries except @v- ; NULL unless there is an @v- */
  Hash *hivalues; 	/* Inherited values from parent @sign's hventry */
  struct sl_inst **lists;
  int nlists;
  struct sl_inst **values;
  int nvalues;
  struct sl_inst **ivalues; /* inherited values */
  int nivalues;
};

struct sl_inst
{
  const char *iid;		/* ID for this instance */
  char type; /* S = signlist; d = listdef; y = sysdef; s = sign; f = form; l = list; v = value; L = linkdef */
  union {
    struct sl_signlist *S;
    struct sl_listdef *d;
    struct sl_sysdef *y;
    struct sl_sign *s;
    struct sl_form *f;
    struct sl_list *l;
    struct sl_linkdef *L;
    struct sl_value *v; } u;
  struct sl_lv_data *lv; 	/* used by form instances */
  struct sl_inst *parent_s; 	/* The parent sign for a form or value instance; if NULL use parent_f */
  struct sl_inst *parent_f; 	/* The parent form for a value instance */
  struct tis_data *tp;		/* The statistics for the instance as imported from .tis by sx_idata_init */
  struct tis_data *ftp;		/* The statistics for a sign and its forms */
  struct tis_data *mtp;		/* The statistics for a sign which is a merge head */
  List *lp;			/* Lemmata written with the sign-value; very preliminary implementation */
  List *notes;			/* A list of struct sl_note * */
  List *sys;			/* A list of @sys in a sign or form */
  List *links;			/* A list of @link in a sign or form */
  const char *lang; 	  	/* this is inline in the @v; an x-value could have a lang with one sign but not another */
  unsigned const char *key;	/* SIGN.FORM.VALUE key for this inst */
  const char *atoid;		/* An OID given with @oid */
  Mloc mloc;
  Mloc eloc;			/* Mloc for @end sign or @@ */
  Boolean valid; /* doesn't have a - after it */
  Boolean inherited;
  Boolean literal;
  Boolean deflt; /* @form+ */
  Boolean query;
  Boolean upua;
  Boolean utf8;
  Boolean uhex;
  Boolean umap;
  Boolean useq;
  Boolean uname;
  Boolean urev;
};

struct sl_linkdef
{
  unsigned const char *name;
  const char *comment;
  struct sl_inst inst;
};

struct sl_link
{
  const char *name;  /* linkdef name, e.g., eBL, Wikidata */
  unsigned const char *label; /* label for URL */
  unsigned const char *url;   /* URL */
  struct sl_inst *ip;
};

struct sl_listdef
{
  unsigned const char *name;
  const char **names;
  int nnames;
  int sorted;
  Hash *known;
  Hash *seen;
  const char *str;
  struct sl_inst inst;
};

struct sl_sysdef
{
  unsigned const char *name;
  const char *comment;
  struct sl_inst inst;
};

struct sl_sys
{
  const char *name;
  const char *subname; /* post-colon name part */
  unsigned const char *v;
  unsigned const char *vv;
  struct sl_inst *ip;
};

struct sl_letter
{
  const unsigned char *name;
  int code; 		/* integer of first character; used in
			   directory names, e.g., l0065, l0352, but
			   not suitable for sorting */
  const char *lname;	/* letter name set for number_group */
  const char *xmlid; 	/* formatted code for use as xml:id */
  Hash *hgroups;	/* hash of groups used while building structure */
  struct sl_group *groups;
  int ngroups;
};

struct sl_group 
{
  const unsigned char *name;
  Hash *hentry; 	/* Hash of sl_inst *that belong to this group */
  struct sl_inst **signs;
  int nsigns;
};

struct sl_compound
{
  char initial_or_final; /* 0 for no; -1 for initial; 1 for final */
  char medial; 		 /* 0 for no; 1 for singleton occurrence; 2
			    for multiple occurences. This means that
			    if a final instance of the sign is
			    encountered member can be decremented and
			    will zero out if the final member is also
			    the only medial member, but remain 1 if
			    the sign is, e.g., |U.U.U| */
  char container; 	 /* 0 for no 1 for yes */
  char contained; 	 /* 0 for no 1 for yes */
};

/* These are NULL-terminated arrays of OID or compound-sign-names
   which are derived from the sl_compound data */
struct sl_compound_digest
{
  const char **memb;
  const char **initial;
  const char **medial;
  const char **final;
  const char **container;
  const char **contained;
};

struct sl_number
{
  const unsigned char *set;
  const unsigned char *rep;
  const unsigned char *ref; /* the eventual xml:id of a sign or form */
  const unsigned char *oid; /* the oid regardless of whether this is a sign or form */
  struct sl_token *t;
  int setsort; /* sl_token t->s is sort code for entire grapheme;
		  setsort is sort code for bare 'set' member */
  const char *sset;	    /* stylistic set to apply to number when displaying with CSS */
};

struct sl_numset
{
  int from; /* first number in set group */
  int last; /* last number in set group */
};

struct sl_sign
{
  struct sl_signlist *sl;
  const unsigned char *name;
  int name_is_listnum;
  Hash *hlentry; 	/* All @list entries */
  Hash *hventry;	/* All @v entries */
  Hash *hvbases;	/* All @v bases; used only for checking duplicates like a₂ and a₃ within a @sign */
  Hash *hfentry;	/* All @form entries */
  Hash *hcompounds;	/* Compound data: sign S has hash of names of
			   compounds C with hashvals consisting of
			   struct sl_compound; two special items are
			   hashed: '#digest_by_oid' --and
			   '#digest_by_name'--#digest_by_name not currently implemented;
			   the hashvals for special items are struct sl_compound_digest * */
  Hash *h_ligas;	/* Hash for @liga by name token */
  struct sl_letter *letter;
  unsigned const char *group;
  struct sl_inst **lists;
  int nlists;
  struct sl_inst **values;
  int nvalues;
  struct sl_inst **forms;
  int nforms;
  struct sl_unicode U;
  const unsigned char *pname; 	/* For type=sign this is the plus-name
				   (e.g., |GA₂×A+HA| = |GA₂×(A.HA)|);
				   for other types it is the map value
				   from, e.g., @sref BA => BU */
  List *aka;			/* alternatively known as sign-names
				   to support non-standard names; data
				   type is Memo_str with literal flag
				   stored in user ptr */
  List *as_form;		/* List of instances where this sign
				   occurs as a form of another sign */
  struct sl_inst *formdef;	/* The defining instance of a form */
  int sort;
  size_t count;
  size_t mcount;
  size_t ctotal;
  size_t mctotal;
  const char *oid;
  const char *smap;	   	/* Must be a @form within this sign */
  const char *smoid;	   	/* OID for @form in @smap */
  const char *moid;		/* OID for destination if this sign is to be merged with another */
  struct sl_sign *msign;	/* sl_sign * for merge destination */
  List *merge;			/* List of sign names to be merged into this sign */
  List *script;			/* List of @script entries turned into sl_scriptdata structs */
  struct sl_inst *inst;
  struct sl_form *xref;        	/* this sign is a header for the @form
				   which defines the sign name; sort
				   value is in sort sequence with
				   signs; note that the form might
				   occur in multiple signs (it will
				   only get added to the global signs
				   hash once) so printed xrefs should
				   use sign->xref->form->owners which
				   should also be sorted before
				   output */
  const char *deep;		/* deep sig generated via unicode_seq */
  Boolean fake;
  enum sx_tle type;
#if 0
  Mloc *mloc; /* Or: keep this as indicator of "defining instance" ? */
#endif
};

struct sl_form
{
  const unsigned char *name;
  const unsigned char *pname;
  struct sl_sign *sign; /* This always points to a sign that
			   corresponds to the form; if the form
			   doesn't occur as an @sign entry, the
			   back-reference form->sign->xref is set */
  Hash *h_ligas;	/* Hash for @liga by name token */
  List *owners; 	/* this is a list of sl_sign* the form is associated with */
  struct sl_sign **owners_sort; /* owners as sorted array */
  int nowners;
  List *insts; 		/* this is a list of sl_inst* where the form occurs */
  List *aka;		/* alternatively known as form-names to support non-standard names, data type is Memo_str */
  List *sys; 		/* List of @sys entries in sign */
  int name_is_listnum;
  int sort;
  const char *oid;
  struct sl_unicode U;
  const char *deep;		/* deep sig generated via unicode_seq */
  Boolean compoundonly;
};

enum sl_ll_type { sl_ll_none , sl_ll_list , sl_ll_lref };
struct sl_list
{
  const unsigned char *name;
  const unsigned char *base;
  const unsigned char *num;
  const unsigned char *sname;
  const unsigned char *ucun;
  const unsigned char *imagefile;
  const unsigned char *values;
  const unsigned char *feat; /* OpenType features string, e.g., .1
				.ss01 .cv32; concatenated to U+ value
				for @list entry */
  int sort;
  List *insts; 		/* signs or forms where this list occurs */
  const char **oids; 	/* NULL-terminated, sorted and uniqued list of
			   sign/form names from the list's insts */
  struct sl_inst *inst;
  enum sl_ll_type type; /* sl_ll_list or sl_ll_lref */
};

/* This is the global value information structure */
struct sl_value
{
  const unsigned char *name;
  const unsigned char *base; /* without index, e.g., for a₃ this is 'a' */
  struct sl_sign *sowner; /* for a value at the sign level, this is
			     the sign it belongs to; may be NULL if
			     value only occurs in forms */
  List *fowners; 	  /* for a value at the form level, this is a
			     list of sl_inst* it belongs to */
  List *insts;
  struct sl_parents *parents; /* Just the valid parents computed from the insts */
  struct sl_inst **fowners_i_sort; /* The fowners as an array of
				      sorted pointers to sl_insts */
  int nfowners;
  int sort;
  const char **oids; 	  /* NULL-terminated, sorted and uniqued list of
			     sign/form names from the value's sign and form
			     insts */
  Boolean atf;		  /* value is not a simple grapheme but an atf transliteration, e.g., u-gun₃ */
  Boolean xvalue;      	  /* value ends in ₓ */
  Boolean qvsign;	  /* value must be qualified when it has a sign parent */
  Boolean qvform;	  /* value must be qualified when it has a form parent */
  Boolean qvmust;	  /* Value has no unqualified version */
  Boolean unknown;     	  /* name is 'x'; these are SIGN entries in
			     lex whose value is not preserved */
  unsigned char index; 	  /* 1 for no index; integer value of index for numeric indices; 255 for sub x */
};

struct sl_scriptdef
{
  const char *name; 	/* Oracc script-style name */
  const char *sset; 	/* OpenType stylistic set as base */
  List *codes; 		/* List of sl_scriptdata* */
};

struct sl_scriptdata
{
  struct sl_sign *sign; /* sign where @script occurred; if within @form this is the form-sign */
  const char *name;	/* script-style name */
  const char *sset;	/* explicitly requested ss00 code; defaults to code of script-style */
  const char *cvnn;	/* cvnn code for this sign in this script-style */
  const char *code;	/* U+ codepoint */
  const char *salt;	/* salt integer as string */
  const char *merge; 	/* sign this code merges with */
  const char *oivs;	/* Oracc IVS as simple hex, e.g., E0100 */
};

struct numvmap_tab
{
  const char *name;
  const char *asif;
};
extern struct numvmap_tab *numvmap (register const char *str, register size_t len);

struct sl_config
{
  int configged;	/* did we read a config file yet */
  const char *project;  /* set from -P[PROJECT] */
  const char *signlist; /* default: csl */
  const char *domain; 	/* default: sl */
  const char *font;	/* default: noto */
  const char *script;	/* default: empty; this is preferred over font
			   because it is used by various sxweb and
			   cuneify routines to display characters with
			   the script-appropriate glyph */
  const char *merge;	/* default: 00lib/csl.mrg */
  int nokeys;
};

extern void asl_config(const char *project, struct sl_config *cp);
extern struct sl_signlist *asl_bld_init(void);
extern void asl_bld_listdef(Mloc *locp, struct sl_signlist *sl, const char *name, const char *in);
extern void asl_bld_scriptdef(Mloc *locp, struct sl_signlist *sl, char *text);
extern void asl_bld_sysdef(Mloc *locp, struct sl_signlist *sl, const char *name, const char *comment);
extern void asl_bld_images(Mloc *locp, struct sl_signlist *sl, const unsigned char *n);
extern void asl_bld_form(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,int minus_flag);
extern void asl_bld_list(Mloc *locp, struct sl_signlist *sl, const char *listlref, const unsigned char *n, int minus_flag, const unsigned char *feat, unsigned const char *sname, unsigned const char *uniimg, unsigned const char *values);
extern int asl_bld_num(Mloc *locp, struct sl_signlist *sl, const uchar *n, struct sl_token *tokp, int priority);
extern void asl_bld_aka(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_smap(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_pname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_comp(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list);

extern struct sl_list* asl_bld_tle(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, const unsigned char *m, enum sx_tle type);
extern void asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,int minus_flag);
extern void asl_bld_pcun(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,int minus_flag);
extern void asl_bld_xsux(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,int minus_flag);
extern void asl_bld_signlist(Mloc *locp, const unsigned char *n, int list);
extern void asl_bld_sys(Mloc *locp, struct sl_signlist *sl, const char *sysname, unsigned const char *v, unsigned const char *vv);
extern void asl_bld_term(struct sl_signlist *);
extern struct sl_token* asl_bld_token(Mloc *locp, struct sl_signlist *sl, unsigned char *t, int literal);

extern void asl_bld_uhex(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_urev(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_useq(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_upua(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_utf8(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_umap(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_uname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_unote(Mloc *locp, struct sl_signlist *sl, const unsigned char *t);
extern void asl_bld_value(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
			  const char *lang, int atf_flag, int minus_flag);
extern void asl_register_sign(Mloc *locp, struct sl_signlist *sl, struct sl_sign *s);

extern void asl_bld_note(Mloc *locp, struct sl_signlist *sl, const char *tag, const char *txt);
extern void asl_bld_merge(Mloc *locp, struct sl_signlist *sl, const unsigned char *n);
extern void asl_bld_script(Mloc *locp, struct sl_signlist *sl, char *txt);
extern void asl_bld_oid(Mloc *locp, struct sl_signlist *sl, const unsigned char *n);

extern void asl_bld_end_form(Mloc *locp, struct sl_signlist *sl);
extern void asl_bld_end_sign(Mloc *locp, struct sl_signlist *sl, enum sx_tle type);
extern void asl_bld_end_pcun(Mloc *locp, struct sl_signlist *sl, enum sx_tle type);
extern void asl_bld_end_xsux(Mloc *locp, struct sl_signlist *sl, enum sx_tle type);
extern struct sl_sign *asl_form_as_sign(struct sl_signlist *sl, struct sl_form *f);

extern unsigned const char *asl_make_key(Mloc *locp, struct sl_signlist *sl, const char *isp, const char *ifp, const char *ivp);
extern void asl_add_key(Mloc *locp, struct sl_signlist *sl, struct sl_inst *hval, const char *s, const char *f, const char *v);

extern void asl_bld_linkdef(Mloc *locp, struct sl_signlist *sl, const char *name, const char *comment);
extern void asl_bld_link(Mloc *locp, struct sl_signlist *sl, const char *sysname, unsigned const char *v, unsigned const char *vv);

extern void asl_bld_liga(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, const unsigned char *fseq, const unsigned char *u);

extern struct sl_config *asl_get_config(void);

#endif/*SIGNLIST_H_*/
