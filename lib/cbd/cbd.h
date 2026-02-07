#ifndef CBD_H_
#define CBD_H_ 1

#include <hash.h>
#include <memo.h>
#include <pool.h>
#include <mesg.h>
#include <form.h>

/* Common grammar header file for cbd.l and cbd.y */
#include "cbdyacc.h"

#include "cbd.tab.h"

#undef ucc
#define ccp const char *
#define ucc unsigned const char *

#ifndef ucp
#define ucp unsigned char *
#endif

typedef struct cbdset
{
  List *lcbds;
  Hash *hcbds;
  Hash *lngs; /* Hash of signatures by lang field */
  Hash *lems; /* Hash of loaded lemm-*.sig by lang element of file name */
  Hash *cof_heads;
  Hash **cof_tails;
  Hash *hsiglangs;
  int ntails;
  List *parts;
  /*Hash *psus;*/
  Memo *cbdmem;
  Memo *cofmem;
  Memo *cformsmem;
  Memo *lsigmem;
  Pool *pool;
} Cbds;

extern Cbds *csetp;

typedef struct cbd {
  Mloc l;
  const char *file;
  unsigned const char *project;
  unsigned const char *lang;
  unsigned const char *name;  /* this is the name as given in @name */
  unsigned const char *trans;
  unsigned const char *version;
  unsigned const char *iname; /* this is the internal name used as the hash key in cbds */
  struct xpd *xpd;
  List *letters;
  List *entries;
  List *edits;
  List *propss;
  List *reldefs;
  Hash *hentries;
  Hash *hsenses;
  Hash *haliases;
  Hash *simple;
  char *i18nstr; 	/* content of the @i18n header tag */
  List *i18n;		/* list of i18n langs from @i18n tag in header */
  Memo *aliasmem;
  Memo *allowmem;
  Memo *cgpmem;
  Memo *editmem;
  Memo *entrymem;
  Memo *equivmem;
  Memo *i18nmem;
  Memo *kisnullmem;
  Memo *loctokmem;
  Memo *locatormem;
  Memo *metamem;
  Memo *metaordermem;
  Memo *partsmem;
  Memo *pleiadesmem;
  Memo *sensesmem;
  Memo *tagmem;
  Memo *taglmem;
} Cbd;

/* Store translated data */
struct i18n {
  Mloc l;
  const char *lang;
  void *data;
};

struct loctok {
  Mloc l;
  unsigned char *tok;
  const char *lang;
};

typedef struct cgp {
  unsigned const char *cf;
  unsigned const char *gw;
  unsigned const char *pos;
  unsigned const char *tight;
  unsigned const char *loose;
  struct entry *owner;
} Cgp;

enum edit_t { ADD_E, ADD_S, DEL_E, DEL_S, REN_E, REN_S, MRG_E, MRG_S, TOP };

struct edit {
  Mloc *lp;
  enum edit_t type;
  struct cgp *target;
  unsigned char *why;
  struct sense *sp;
  int force;
  void *owner;
};

typedef enum efield {
  EFLD_PERD,
  EFLD_FORM,
  EFLD_NORM,
  EFLD_NMFM, /* ^ is the field code for $NORM=FORM */
  EFLD_FMOF, /* ~ is the field code for FORM~~OFORM */
  EFLD_BASE,
  EFLD_CONT,
  EFLD_STEM,
  EFLD_MRF1,
  EFLD_MRF2,
  EFLD_TOP
} Efield;

#include <kis.h>

typedef Hash *Hfields[EFLD_TOP];
typedef Kis_data Kfields[EFLD_TOP];

/* Wrapper structure to help manage forms as they occur in CBD @form */
typedef struct cform {
  Mloc l;
  const char *id;
  struct entry *e;
  struct sense *s;
  Form f;  
} Cform;

typedef struct entry {
  Mloc l;
  Kis_data k;
  struct cgp *cgp;
  const char *eid;
  const char *lang;
  Hash *b_pri;
  Hash *b_alt;
  Hash *b_sig;
  Hash *b_allow;
  List *allows; /* struct allow * */
  List *aliases;
  Hash *stems;
  List *dcfs;
  Hash *hdcfs;
  List *parts; /* list of struct parts * */
  List *bases; /* list of base components in @bases; list data is
		  another list, first element is pri, rest are alt
		  pri/alt data are struct loctok
	        */
  List *forms;
  List *senses;
  Mloc *begin_senses;
  Mloc *end_senses;
  Mloc *end_entry;
  Hash *hsenses; /* needed for building cbd from sigs */
  union
  {
    Hfields  *hfields;
    Kis_data **kisdata;
  } u;
  struct tag *phon;
  struct tag *root;
  struct tag *disc;
  int rank;
  int usage;
  int compound;
  struct cbd *owner;
  struct edit *ed;
  struct meta *meta;
  List *gwls; /* list of struct i18n with (char*) data */
  List *discls; /* list of struct i18n with (char*) data; one day this will be OTF inline tree */
} Entry;

struct allow {
  Mloc l;
  unsigned char *lhs;
  unsigned char *rhs;
  unsigned char *lsig;
  unsigned char *rsig;
};

typedef struct cof
{
  unsigned char *s;
  struct entry *e;
  struct cform *f;
  int i;
} Cof;

struct meta {
  List *order;
  List *bib;
  List *collo;
  List *equiv;
  List *inote;
  List *isslp;  
  List *note;
  List *oid;
  List *prop;
  List *pleiades;
  List *rel;
};

struct metaorder {
  Mloc l;
  int tok;
  const char *name;
  void *val;
};

struct alias {
  Mloc l;
  struct cgp *cgp;
};

typedef struct parts {
  Mloc l;
  List *cgps; /* list_free(cgps,NULL) when freeing cbd */
  Entry *owner;
} Parts;

typedef struct sense {
  Mloc l;
  Kis_data k;
  unsigned const char *num;
  unsigned const char *sgw;
  unsigned const char *pos;
  unsigned const char *lng;
  unsigned const char *mng;
  unsigned const char *sid;
  struct tag *disc;
  int rank;
  struct entry *owner;
  struct edit *ed;
  struct meta *meta;
  List *sensels; /* list of translated senses with (struct sense *) data--struct has l and lng so i18n not needed */
  List *forms; /* not yet used: future provision for tying @form under @sense */
} Sense;

struct equiv {
  Mloc l;
  unsigned char *lang;
  unsigned char *text;
};

struct isslp {
  Mloc l;
  unsigned char *year;
  unsigned char *text;
  struct entry *owner;
};

struct pleiades {
  Mloc l_id;
  Mloc l_coord;
  unsigned char *id;
  unsigned char *coord;
  List *pl_aliases; /* list of struct loctok * so pl_aliases could use %lang */
};

struct cbdpos {
  const char *name;
  int val;
};

struct cbdrws {
  const char *name;
  const char *lang;
};

struct tag {
  Mloc l;
  const char *name;
  unsigned char *val;
};

struct tagl {
  Mloc l;
  unsigned char *data;
  List *i18n; /* List of i18ns translating this tag; i18n->data is char* */
};

struct cbdtag {
  const char *name;
  int tok;
  int eol_sp;
  int ss;
};

typedef struct lemsig
{
  unsigned const char *sig;
  int rank;
  int freq;
} Lemsig;

typedef enum form_walk_type {
  CBD_FW_E , 	/* entry */
  CBD_FW_EE , 	/* end entry */
  CBD_FW_S , 	/* sense */
  CBD_FW_SE , 	/* end sense */
  CBD_FW_EF , 	/* entry-level form */
  CBD_FW_SF , 	/* sense-level form */
  CBD_FW_PE , 	/* PSU entry */
  CBD_FW_PS  	/* PSU sense */
} Cbd_fw_type;

typedef void (*cbdfwfunc)(Cform *,Cbd_fw_type,void *);

typedef void (*cbdactionfunc)(const char *,int,int,void*);
extern void cbd_key_set_action(cbdactionfunc f);
extern void cbd_key_cgp(Cform *f, Entry *e, const char *period);
extern void cbd_key_cgpse(Cform *f, Sense *s, const char *period);
extern void cbd_key_fields(Cform *f, int context, void *v);

extern Sense *curr_sense;
extern const char *errmsg_fn;

/*extern Hash *cbds;*/
extern struct cbd *curr_cbd;
extern struct entry *curr_entry;
extern struct alias *curr_alias;
extern struct parts *curr_parts;

extern struct cbdpos *cbdpos(const char *str, size_t len);
extern struct cbdrws *cbdrws(const char *str, size_t len);
extern int edit_script(struct cbd *c);
extern void cgp_parse(struct cgp *c, unsigned char *s, Mloc *lp);
extern unsigned char *cbd_form_sig(struct entry *e, Form *f2p);

extern struct cbdtag *cbdtags(const char *str, size_t len);
extern List *cgp_get_all(void);
extern struct cgp*cgp_get_one(void);
extern void cgp_save(unsigned char *cf, unsigned char *gw, unsigned char *pos);
extern struct loctok *loctok(YYLTYPE *lp, struct entry *e, unsigned char *tok);

extern struct alias *cbd_bld_alias(YYLTYPE l, struct entry *e);
extern void cbd_bld_allow(YYLTYPE l, struct entry *e, unsigned char *lhs, unsigned char *rhs);
extern void cbd_bld_bases_pri(YYLTYPE l, struct entry *e, unsigned char *lang, unsigned char *p);
extern void cbd_bld_bases_alt(YYLTYPE l, struct entry *e, unsigned char *a);
extern struct cbd *cbd_bld_cbd(void);
extern void cbd_bld_cbd_setup(struct cbd *c);
extern void cbd_bld_cbd_term(struct cbd *c);
extern List *cbd_bld_cmt_append(List *to, List *from);
extern void cbd_bld_cmt_queue(Mloc *lp, unsigned char *cmt);
extern void cbd_bld_dcf(YYLTYPE l, struct entry *e, unsigned char *dcf, unsigned char *dcfarg);
extern void cbd_bld_discl(YYLTYPE l, struct entry *e, const char *lang, unsigned char *text, int e_or_s);
extern void cbd_bld_edit(struct entry *e, char ctxt, char type);
extern void cbd_bld_edit_entry(struct entry *e, char type);
extern struct sense *cbd_bld_edit_sense(struct entry *e, char type);
extern void cbd_bld_edit_why(struct entry *e, char *why);
extern struct entry *cbd_bld_entry(YYLTYPE l, struct cbd *c);
extern void cbd_bld_entry_cgp(struct entry *e);
extern struct equiv *cbd_bld_equiv(struct entry *e, unsigned char *lang, unsigned char *text);
extern Cform *cbd_bld_form(YYLTYPE l, struct entry *e);
extern void cbd_bld_form_setup(struct entry *e, Cform *fp);
extern void cbd_bld_gwl(YYLTYPE l, struct entry *e, const char *lang, unsigned char *text);
extern Mloc *cbd_bld_locator(YYLTYPE lp);
extern struct loctok *cbd_bld_loctok(YYLTYPE *lp, struct entry *e, unsigned char *tok);
extern struct meta *cbd_bld_meta_create(struct entry *e);
extern void cbd_bld_meta_add(YYLTYPE l, struct entry *e, struct meta *mp, int tok, const char *name, void *val);
extern void cbd_bld_note(YYLTYPE l, struct entry *e, struct meta *curr_meta, unsigned char *text);
extern void cbd_bld_notel(YYLTYPE l, struct entry *e, struct meta *curr_meta, const char *lang, unsigned char *text);
extern struct parts *cbd_bld_parts(YYLTYPE l, struct entry *e);
extern struct pleiades *cbd_bld_pl_id(YYLTYPE l, struct entry *e, unsigned char *id);
extern void cbd_bld_pl_coord(YYLTYPE l, struct pleiades *p, unsigned char *coord);
extern void cbd_bld_pl_alias(YYLTYPE l, struct pleiades *p, const char *lang, unsigned char *alias);
extern void cbd_bld_props(struct cbd *c, char *text);
extern void cbd_bld_reldef(struct cbd *c, char *text);
extern struct sense *cbd_bld_sense(YYLTYPE l, struct entry *e);
extern struct sense *cbd_bld_sensel(YYLTYPE l, struct entry *e);
extern void cbd_bld_stem(YYLTYPE l, struct entry *e, unsigned char *stem);
extern struct tag *cbd_bld_tag(YYLTYPE l, struct entry *e, const char *name, unsigned char *val);
extern void cbd_no_form_bases(Entry *ep);
extern void cbd_cof_register(Cform *cfp);
extern void cbd_psu_register(Entry *ep, Parts *pp);
extern void cbd_end_sense(void);

extern void cbd_sig_add_one(const unsigned char *s, int rank);
extern void cbd_sig_add_list(List *lp);

extern void cgp_init(struct cgp *c, unsigned char *cf, unsigned char *gw, unsigned char *pos);
extern unsigned const char *cgp_entry_str(struct entry *e, int spread);
extern const unsigned char *cgp_str(struct cgp *cp, int spread);
extern void cgp_entry(struct cgp *c, struct entry *e);
extern void cbd_psus(void);
extern void cgp_set_pool(Pool *p);

extern void cbd_form_walk(Cbd *c, cbdfwfunc h);
extern void cbd_kis(Cbd *c, Kis *k);

#endif/*CBD_H_*/
