#ifndef TLOC_H_
#define TLOC_H_

struct loch_cbd
{
  struct gloc *c; /* This tracks the current CBD (glossary) info */
  struct aloc *a; /* This tracks alocs->last */
  List *alocs;
};

struct loch_xtf
{
  struct tloc *t; /* This tracks the current T(ext) info */
  struct lloc *l; /* This tracks t->llocs->last */
  struct wloc *w; /* This tracks t->llocs->last->wlocs->last */
  List *tlocs;
  union
  {
    struct gsig *g; /* This tracks t->llocs->last->wlocs->last->gsigs->last */
    struct lsig *l; /* As above but for lemma signatures */
    struct msig *m; /* As above but for multi-lemma signatures */
  } u;
};

/* Location header: one for each input run */
typedef struct loch
{
  struct trun *r; 	/* This is the run--normally a set of inputs belonging to the same project */
  const char *project;	/* This is the project whose data is being processed */
  const char *type;
  union
  {
    struct loch_cbd c;
    struct loch_xtf x;
  } u;
} Loch;

/* Text location: one for each (XTF) text */
typedef struct tloc
{
  const char *file;		/* for Loch->type == "xtf" this is
				   actually the ATF file/line from the
				   XTF PIs */
  int andline_num;		/* need to check that this is emitted
				   by ATF-processor */
  const char *text_project;  	/* This is the project where the
				   current text lives; because of
				   proxying this can be different from
				   Loch's project */
  const char *text_id;
  const char *text_name;
  const char **keys;	/* [0]=k,[1]=v; NULL-terminated */
  int nkeys; 		/* number of char * in keys not counting terminating NULL */
  struct loch *loch;
  List *llocs;
} Tloc;

typedef struct lloc
{
  const char *line_id;
  const char *line_num;
  const char *line_label;
  Tloc *t;
  List *wlocs;
} Lloc;

typedef struct wloc
{
  const char *word_id;
  const char *word_lang;
  const char *word_form;
  Lloc *l;
  List *gsigs;		/* NULL unless r->multi is in effect */
  struct trun_word *w;
} Wloc;

#define loch_cbd(r)	((r)->l->u.c)

#define loch_glo(r)	((r)->l->u.c.c)
#define loch_art(r)	((r)->l->u.c.a)

#define loch_xtf(r)	((r)->l->u.x)

#define loch_text(r)	((r)->l->u.x.t)
#define loch_line(r)	((r)->l->u.x.l)
#define loch_word(r)	((r)->l->u.x.w)

#define loch_gsig(r)	((r)->l->u.w->u.g)
#define loch_lsig(r)	((r)->l->u.w->u.l)
#define loch_msig(r)	((r)->l->u.w->u.m)

#endif/*TLOC_H_*/
