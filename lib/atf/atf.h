#ifndef ATF_H_
#define ATF_H_

#include "cat.h"

#include "blocktok.h"
typedef enum e_type Atflt;

typedef enum linkt { ELINK_DEF ,  ELINK_SOURCE , ELINK_PARALLEL , ELINK_TOP } Linkt;

typedef enum prot { PROT_LZR_SPARSE ,  PROT_LZR_STOP , PROT_BIB , PROT_NOTE ,
		    PROT_VERSION , PROT_ATF , PROT_PROJECT , PROT_TOP } Prot;

typedef enum linet { LINE_MTS , LINE_NTS , LINE_LGS, LINE_GUS ,
		     LINE_LEM , LINE_LINK , LINE_COMMENT , LINE_DOLLAR ,
		     LINE_BLANK , LINE_TOP } Linet;

typedef enum doct {
  EDOC_NONE , EDOC_TRANSLITERATION , EDOC_COMPOSITE , EDOC_SCORE ,
  EDOC_MATRIX , EDOC_SYNOPSIS , EDOC_PARSED , EDOC_UNPARSED , EDOC_WORD
} Doct;

typedef enum wheret { WH_NONE , WH_PREAMBLE , WH_GROUP } Wheret;

/* Management structure for ATF texts */
typedef struct atfm {
  List *lprotocols; /* repeatedly reset to array */
  List *llinks;
  List *llines;
  List *lkeys;
  Memo *mblocks;
  Memo *mxlinks;
  Memo *mlines;
  Memo *mkeys;
  Memo *matfl;
  Memo *mprotocols;
  Memo *mgroups;
  Pool *pool;
  struct atf *atf;
} Atfm;

/* Data structure for ATF texts */
typedef struct atf {
  struct atfm *man;
  const char *file;
  struct atfl *src; /* &-line */
  List *atflines; /* list of Atfl pointers to the data for each line, to drive output */
  ccp pqx;
  uccp name;
  enum doct edoc; /* doctype as enum */
  enum doct stype;
  enum doct sparse;
  enum doct sword;
  uccp project;
  const char *lang;
  const char *altlang;
  const char *script;
  struct protocol **protocols;
  int nprotocols;
  int flags;
  Hash *lzr_sparse;
  struct xlink **links;
  int nlinks;
  struct key **keys;
  int nkeys;
  struct group *lines; /* attach point for #- and $-lines that occur before first MTS */
  Tree *body; /* probable implementation of document hierarchy */
} ATF;

typedef struct atfl {
  Mloc l;
  Atflt t;
  union {
    struct atf *atf;
    ccp *	doc;
    uccp *	prj;
    struct link*lnk;
    struct key* key;
    struct nonx*nox;
    ccp *	cmt;
    ccp *	nul; /* blank line */
    struct blk* blk; /* milestones/objects */
  } u;
} Atfl;

typedef struct xlink {
  struct atfl *src;
  enum linkt t;
  uccp siglum;
  ccp qid;
  uccp name;
} Xlink;

/* Ported from otf/lib/key.[ch] */
typedef struct key {
  struct atfl *src;
  const char *key;
  const char *val;
  const char *url;
} Key;

/* This is used for the following protocols:
 *
 * #lemmatizer: do sparse <FIELDS>
 *
 * #lemmatizer: stop <INTEGER>
 *
 * #bib: <STRING>
 *
 * #note: <STRING>
 *
 * #version: <STRING>
 *
 * It could also be used for #atf: use but they are currently handled as flags.
*/
typedef struct protocol {
  struct atfl *src;
  enum prot t;
  const char *type;
  union {
    int stop;
    const unsigned char *str;
    Hash *sparse;
  } u;
} Protocol;

/* nonx (mainly $-line) implementation ported from ox */
#include "nonx.h"

/* These are the possible child nodes in a block hierarchy */
typedef struct block {
  struct blocktok *b;
  struct group *lines;
  void *v; /* used by nonx to store Nox ptr *//*probably not as it turns out */
} Block;

#if 0
typedef struct blk Block;
typedef struct blk Column;
typedef struct blk Div;
typedef struct blk Milestone;
typedef struct blk Object;
typedef struct blk Surface;
#endif

typedef struct group {
  struct block *parent; /* this is a locator group */
  struct line **lines; /* these are gathered in atfmp->llines */
  int nlines;
} Group; /* line-group */

typedef struct line {
  Linet t;
  Node *gdl;
} Line;

#define ATFF_MYLINES 0x01
#define ATFF_AGROUPS 0x02
#define ATFF_MATH 0x04
#define ATFF_UNICODE 0x08
#define ATFF_LEGACY 0x10
#define ATFF_LEXICAL 0x20
#define ATFF_TOP 0x40

#define AP_ATTR 1	/* for props; GP_xxx is more complex but ATF
			   may only need this and can use PG_XML as
			   type arg */

extern ATF *atfp;
extern Atfm *atfmp;

extern Block *curr_block;
extern Blocktok *curr_blocktok;
extern Group *curr_group;

extern int atflineno;
extern const char *atffile, *curratffile;
extern int in_preamble;
extern const char *curr_use_str;

#define atf_xprop(xnp,xk,xv) atf_prop_kv(xnp,AP_ATTR,PG_XML,xk,xv)

extern int atfparse(void);
extern Tree *atf_read(const char *);
extern Tree *atfyacc(void);
extern void atf_lex_init(FILE *fp, const char *file);
extern void atf_protocol(const char *p);
extern void atf_init(void);
extern void atf_term(void);
extern void atf_wrapup(Wheret where);

extern void atf_bld_tree(Tree *tp);
extern void atf_set_tree(Tree *tp);
extern void atf_wrapup_buffer(void);

extern void atf_lang(ATF *a, const char *atf_lang);
extern void atf_bld_amp(Mloc l, const char *pqx, unsigned const char *name);
extern void atf_bld_doc(Mloc l);
extern void atf_bld_implicit_block(void);
extern void atf_bld_key(Mloc l, char *str);
extern void atf_bld_link(Mloc l, Linkt lt, const unsigned char *siglum,
			 const char *qid, const unsigned char *name);
extern void atf_bld_protocol(Mloc l, Prot pt, const char *s);
extern void atf_bld_atf_protocol(Mloc l, int usetype, const char *str);

extern void atf_bld_column(Mloc l, Blocktok *curr_blocktok);
extern void atf_bld_division(Mloc l, Blocktok *curr_blocktok);
extern void atf_bld_heading(Mloc l, Blocktok *curr_blocktok);
extern void atf_bld_milestone(Mloc l, Blocktok *curr_blocktok);
extern void atf_bld_object(Mloc l, Blocktok *curr_blocktok);
extern void atf_bld_surface(Mloc l, Blocktok *curr_blocktok);
extern void atf_bld_mts(Mloc l, int linetype, const char *linetext);
extern void atf_bld_xxx(Mloc l, int linetype, const char *linetext);
extern void atf_prop_kv(Node *ynp, int ptype, int gtype, const char *k, const char *v);

#endif/*ATF_H_*/
