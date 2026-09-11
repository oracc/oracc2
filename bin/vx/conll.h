#ifndef CONLL_H_
#define CONLL_H_

#include <keva.h>
#include <form.h>

typedef struct conll_run
{
  const char *global_columns;
  List *docs;
  Memo *m_doc;
  Memo *m_sent;
  Memo *m_word;
  Pool *pool;
  Pool *pooh;
} Conll_run;

typedef struct conll_doc
{
  const char *doc_id;
  const char *doc_nm;
  struct conll_sent *sents;
  int nsents;
  int sindex;
  struct conll_run *run;
} Conll_doc;

typedef struct conll_sent
{
  const char *par_id;
  const char *sent_id;
  char *text;
  char *tren;
  char *xsux;
  struct conll_word *words;
  int nwords;
  int windex;
  struct conll_run *run;
} Conll_sent;

typedef struct conll_core
{
  const char *ID;
  const char *FORM;
  const char *LEMMA;
  const char *UPOS;
  const char *XPOS;
  const char *FEATS;
  const char *HEAD;
  const char *DEPREL;
  const char *DEPS;
  Keva **MISC;
} Conll_core;

typedef struct conll_plus
{
  const char *PROJECT;
  const char *LANG;
  const char *NORM;
  const char *CF;
  const char *GW;
  const char *SENSE;
  const char *POS;
  const char *EPOS;
  const char *BASE;
  const char *CONT;
  const char *STEM;
  const char *M1;
  const char *M2;
  const char *OID;
  const char *SENSEID;
  const char *FORMC;
  const char *LEMMAC;
  const char *BASEC;
  const char *LEMDIS;
  const char *WORDID;
  const char *LINEID;
  const char *LINELABEL;
} Conll_plus;

typedef struct conll_word
{
  struct conll_core c;
  struct conll_plus p;
  struct conll_run *run;
} Conll_word;

extern Conll_run *conll_init(void);
extern Conll_doc *conll_doc(Conll_run *r, const char *id, const char *nm, int nsents);
extern Conll_sent *conll_sent(Conll_doc *d, size_t nwords);
extern Conll_word *conll_word(Conll_sent *s);

extern void conll_dump(Conll_run *r, FILE *fp);

#endif/*CONLL_H_*/
