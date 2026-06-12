#ifndef GT_H_
#define GT_H_

#include <hash.h>
#include <memo.h>
#include <gdl.h>

/* context-free Grapheme sequence of a grapheme or words data type shared by gx and sx
 */

typedef struct gdl_config
{
  Hash *h;
  Memo *m;
} Gt_cfg;

typedef struct gdl_token
{
  const unsigned char *t;	/* input token, in SL, sign/form/value/base/list name */
  Node *gdl;			/* token as GDL */
  GS_head *gsh;			/* GDL gsort data */
  const char *gsig;		/* return value from gdlsig run on
				   parsed GDL; NULL if the token was a
				   literal l*/
  const char *sign;
  const char *deep;		/* deep sig from gdlsig */
  struct sl_inst *oid_ip;      	/* in SL, sl_inst * to use for retrieving OID */
  int c;			/* sort code for token */
  int priority;			/* in SL, used for registering nums */
} Gt;

extern const char *gdl_last_bad_sig;

extern int gt_toks_gcmp(const void *a, const void *b);
extern int gt_toks_ccmp(const void *a, const void *b);
extern void gt_config(Hash *h, Memo *m);
extern void gt_codes(void);
extern Gt *gt_token(Mloc *locp, unsigned char *t, int literal, void *user);
extern void gt_init(void);
extern void gt_term(void);
extern unsigned char *gt_snames_of(unsigned const char *oids);
extern void gt_set_oids(Hash *o);
extern void gt_load_oids(const char *dom);
extern Gt *gt_get_token(uccp n);

#endif/*GT_H_*/
