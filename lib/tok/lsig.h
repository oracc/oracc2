#ifndef LSIG_H_
#define LSIG_H_

typedef struct lsig
{
  /* Administrative fields */
  const Uchar *file;
  int lnum;
  Unsigned32 flags;

  /* needs thinking; record instance-explicit in f2 of ilem_form; 
     record matches in f2 from sigset; reset sigset f2 match
     before calling cfnorm_ok ? What are the persistency issues? */
  /*  Unsigned32 match; */  
  Uchar *sig;
  Uchar *tail_sig;
  Uchar *psu_ngram;

  /* The five static signature fields */
  const Uchar *cf;
  const Uchar *gw;
  const Uchar *sense;
  const Uchar *pos;
  const Uchar *epos;

  /* Additional fields which vary with instance and language */
  const Uchar *project;
  const Uchar *exo_project; /* used to save project that exosig comes from; output as exoprj when set */
  const Uchar *exo_lang; /* used to save lang of exosig; output as exolng when set */
  const Uchar *form;
  const Uchar *oform;
  const Uchar *lang;
  struct langcore *core;
  const Uchar *norm;
  const Uchar *base;
  const Uchar *cont;
  const Uchar *morph;
  const Uchar *morph2;
  const Uchar *stem;
  const Uchar *augment; /* need to store +.*ra until we get morphology from cbd */
  const Uchar *restrictor;
  const Uchar *rws;
  
  /* Array of components for COFs & PSUs; N.B.: use flags to 
   * determine if COF or PSU.
   */
  struct lsig **parts;
  uintptr_t cof_id;

  /* Store rank so it can persist across nlcp_rewrite */
  int rank;

  /* If this lsig is a structure within an ilem this pointer is to the enclosing ilem. */
  void *owner;

  /* Data structure for use by wordset lookup (or can ->user field be used for this?) */
  void *words;
  
  /* Support for arbitrary user data extensions */
  void *user;
  
  struct wloc *w; 	/* The word-location for the lsig */
} Lsig;

#endif/*LSIG_H_*/
