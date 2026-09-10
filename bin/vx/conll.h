#ifndef CONLL_H_
#define CONLL_H_

#include <keva.h>
#include <form.h>

typedef struct conll_run
{
  struct *conll_doc;
} Conll_run;

typedef struct conll_doc
{
  struct conll_sent *snts;
  struct conll_doc *next;
} Conll_doc;

typedef struct conll_sent
{
  const char *ucun;
  const char *tlit;
  const char *tlat;
  struct conll_word *wrds;
  struct conll_sent *next;
} Conll_doc;

typedef struct conll_word
{
  struct conll_core c;
  struct conll_plus p;
} Conll_word;

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
  Form f;
  const char *oid;
  const char *senseid;
  const char *formc;
  const char *lemmac;
  const char *basec;
  const char *lemdis;
  const char *wordid;
  const char *lineid;
  const char *linelabel;
} Conll_plus;

#endif/*CONLL_H_*/
