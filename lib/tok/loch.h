#ifndef TLOC_H_
#define TLOC_H_

/* Location header: one for each input run */
typedef struct loch
{
  const char *project;
  const char *type;
  Memo *t_m;
  Memo *l_m;
  Memo *w_m;
  struct trun *r; /* This is the run--normally a set of inputs belonging to the same project */
  struct tloc *t; /* This tracks the current T(ext) info */
  struct lloc *l; /* This tracks t->llocs->last */
  struct wloc *w; /* This tracks t->llocs->last->wlocs->last */
  struct gsig *g; /* This tracks t->llocs->last->wlocs->last->gsigs->last */
  List *tlocs;  
} Loch;

/* Text location: one for each (XTF) text */
typedef struct tloc
{
  const char *file;		/* for ->loch->type == "xtf" this is actually the ATF file/line from the XTF PIs */
  int andline_num;		/* need to check that this is emitted by ATF-processor */
  const char *text_project;  
  const char *text_id;
  const char *text_name;
  const char **keys;
  int nkeys; /* number of keys used */
  int nkeyp; /* number of pointers allocated to ->keys */
  struct loch *loch;
  List *llocs;
} Tloc;

typedef struct lloc
{
  const char *line_num;
  const char *line_id;
  const char *line_label;
  List *wlocs;
  Tloc *t;
} Lloc;

typedef struct wloc
{
  const char *word_id;
  const char *word_form;
  const char *word_lang;
  List *gsigs;
  Lloc *l;
  struct trun_word *w;
} Wloc;

#endif/*TLOC_H_*/
