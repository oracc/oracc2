#ifndef ATF_H_
#define ATF_H_

#include "cat.h"

#include "blocktok.h"
typedef enum e_type Atflt;

typedef enum keyt { KEYT_KEY , KEYT_URL , KEYT_TOP } Keyt;

typedef enum linkt { LINK_DEF ,  LINK_EXEMPLAR , LINK_PARALLEL , LINK_TOP } Linkt;

typedef enum doct { EDOC_TLIT , EDOC_TLAT , EDOC_SCORE , EDOC_COMPOSITE } Doct;

/* Management structure for ATF texts */
typedef struct atfm {
  List *lpstart;
  List *llinks;
  List *lkeys;
  List *lmos;
  Memo *mprotocols;
  Memo *mlinks;
  Memo *mkeys;
  Memo *mlatf;
  struct atf *atf;
} Atfm;

/* Data structure for ATF texts */
typedef struct atf {
  struct atfm *man;
  struct atfl *src; /* &-line */
  List *atflines; /* list of Latf pointers to the data for each line, to drive output */
  ccp pqx;
  uccp name;
  ccp sdoc;  /* doctype as a string */
  enum doct edoc; /* doctype as enum */
  int scoreflags; /* 0x01 = parsed, 0x02 = word */
  uccp project;
  struct protocol *pstart;
  int npstart;
  struct link *links;
  int nlinks;
  struct key *keys;
  int nkeys;
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
    struct mo * mos; /* milestones/objects */
  } u;
} Atfl;

typedef struct xlink {
  struct atfl *src;
  enum linkt t;
  uccp siglum;
  ccp pqx;
  uccp name;
} Xlink;

typedef struct key {
  struct atfl *src;
  enum keyt t;
  struct keva k;
  void *x;
} Key;

/* nonx (mainly $-line) implementation ported from ox */
#include "nonx.h"

/* These are the possible child nodes in a block hierarchy */
typedef struct blk {
  struct atfl *src;
  struct block *b;
  union {
    List *l;		/* use this to build a list of divs/milestones/objects */
    struct blk *b;	/* then convert it to an array for further processing */
  } kids;
  struct atf *a;
  struct blk *up;
  void *v; /* used by nonx to store Nox ptr */
} Blk;

typedef struct blk Column;
typedef struct blk Div;
typedef struct blk Milestone;
typedef struct blk Object;
typedef struct blk Surface;
typedef struct blk Group; /* line-group */
typedef struct blk Line;
typedef struct blk Cell;
typedef struct blk Field;
typedef struct blk Word;

extern int atflineno;
extern const char *atffile, *curratffile;

extern int atfparse(void); /* bison */
extern char *atf_name(struct catchunk *cp, char **data);
extern struct catchunk *atf_read(const char *);
extern struct catchunk *atfyacc(void);
extern void atf_lex_init(FILE *fp, const char *file);
extern void atf_protocol(const char *p);
extern void atf_init(void);
extern void atf_term(void);

#endif/*ATF_H_*/
