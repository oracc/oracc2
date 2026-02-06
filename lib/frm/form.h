#ifndef FORM_H_
#define FORM_H_

#include <stdio.h>
#include <c2types.h>

#define Uchar unsigned char
#include "pool.h"

#define FORM_FLAGS_PSU_STOP    0x00000001
#define FORM_FLAGS_PSU_SKIP    0x00000002
#define FORM_FLAGS_LEM_NEW     0x00000004
#define FORM_FLAGS_SHADOW      0x00000008
#define FORM_FLAGS_COF_NEW     0x00000010
#define FORM_FLAGS_INVALID     0x00000020
#define FORM_FLAGS_CF_QUOTED   0x00000040
#define FORM_FLAGS_NO_FORM     0x00000080
#define FORM_FLAGS_PARTIAL     0x00000100
#define FORM_FLAGS_SAME_REF    0x00000200
#define FORM_FLAGS_COF_HEAD    0x00000400
#define FORM_FLAGS_COF_TAIL    0x00000800
#define FORM_FLAGS_IS_PSU      0x00001000
#define FORM_FLAGS_NGRAM_MATCH 0x00002000
#define FORM_FLAGS_NGRAM_KEEP  0x00004000
#define FORM_FLAGS_LEM_BY_NORM 0x00008000
#define FORM_FLAGS_NORM_IS_CF  0x00010000
#define FORM_FLAGS_NEW_BY_PROJ 0x00020000
#define FORM_FLAGS_NOT_IN_SIGS 0x00040000
#define FORM_FLAGS_EXTENDED_OK 0x00080000
#define FORM_FLAGS_NEW_BY_LANG 0x00100000
#define FORM_FLAGS_COF_INVALID 0x00200000
#define FORM_FLAGS_FROM_CACHE  0x00400000
#define FORM_FLAGS_DEF_FORM    0x00800000
#define FORM_FLAGS_DEF_ESENSE  0x01000000
#define FORM_FLAGS_DEF_ISENSE  0x02000000
#define FORM_FLAGS_READ_ONLY   0x04000000
#define FORM_FLAGS_CLEARED     0x08000000
#define FORM_FLAGS_NEWLITERAL  0x10000000
#define FORM_FLAGS_IMPLICIT    0x20000000

/* This includes '<' because it is the delimiter for post-FORM 
   predicates, and ' ' and '\t' because after ']' no spaces are
   allowed.
 
   N.B.: @, % and : are purposely not included in this set. The
   only entries in this list are those which can delimit fields
   in a signature after the CF[GW]POS.
 */
#define FORM_FIELD_ENDS_STR	"$#/+*\\< \t"

struct ilem_form;

typedef struct form
{
  /* Administrative fields */
  const Uchar *file;
  int lnum;
  Unsigned32 flags;
  
  /* If this f2 is a structure within an ilem this pointer is to the enclosing ilem.
     Note that many f2s are not within ilems, so this pointer will often be null.
   */
  struct ilem_form *owner;

  /* needs thinking; record instance-explicit in form of ilem_form; 
     record matches in form from sigset; reset sigset form match
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
#if 0
  const Uchar *restrictor;
#endif
  const Uchar *rws;
  
  /* Data structure for use by wordset lookup */
  struct w2_set *words;

  /* Array of components for COFs & PSUs; N.B.: use flags to 
   * determine if COF or PSU.
   */
  struct form **parts;
  uintptr_t cof_id; /* This is actually the memory address  of COF head stored as uintptr_t */

  /* Store rank so it can persist across nlcp_rewrite */
  int rank;
  
  /* Support for arbitrary user data extensions */
  void *user;

} Form;

#define fentry(xfp) ((Entry *)(xfp)->entry)

#include "sigs.h"
struct sig_context; /* need this because of interweave of form.h/sigs.h includes */

extern Memo *formsmem;
extern Memo *formspmem;

extern void form_init(void);
extern void form_term(void);

extern int form_parse(const Uchar *file, size_t line, Uchar *lp, Form *formp, Uchar **psu_sense);
extern int form_parse_psu(const Uchar *file, size_t line, Uchar *lp, struct form *formp);

extern unsigned char *form_sig(Pool *p, Form *fp);
extern unsigned char *form_sig_sub(Pool *p, Form *fp);
extern unsigned char *form_psu_sig(Pool *p, Form *fp);

extern int form_alias(struct sig_context *scp, Form *fp, Form *ref_fp);
extern int form_extreme_alias(struct sig_context *scp, Form *fp, Form *ref_fp);

extern void form_inherit(Form *inheritor_form, Form *from_form);
extern void form_clear(Form*f);

extern void form_serialize_form(FILE *f_f2, Form *f);
extern void form_serialize_form_2(FILE *f_f2, struct ilem_form *f);

extern unsigned char *form_cbd(Form *fp, Pool *p, int with_lang);

extern void form_set_xcp(struct xcl_context *xcp);

extern struct map *lang949(register const char *str, register size_t len);

#endif/*FORM_H_*/
