#ifndef TOK_H_
#define TOK_H_

#include <trun.h>
#include <loch.h>
#include <gsig.h>

extern Trun *trun_init(void);
extern void trun_term(Trun *r);
extern Word *trun_word_init(Trun *r);
extern void trun_word_reset(Word *w);
extern void trun_word_term(Word *w);

extern void tloc_init(int many);
extern void tloc_term(void);
extern void tloc_key(const char *k, const char *v);
extern Tloc *tloc_line(char *lp);
extern Tloc *tloc_dup(void);
extern Tloc *tloc_dup_arg(Tloc *dup);
extern Tloc *tloc_get(void);
extern void tloc_show(char c);

extern Gsig *gsb_get(Word *w);
extern Gsig *gsb_get_n(Word *w, int n);
extern Gsig *gsb_new(Trun *r);
extern Gsig* gsig_parse(char *s, Gsig *gp, const char *id_sig_sep);
extern void gsb_add(Trun *r, char type, const char *form, const char *oid,
		    const char *sign, const char *spoid, const char *spsign, const char *lang, 
		    const char *logolang);
extern void gsb_c_last(Word *w);
extern void gsb_last(Trun *r);
extern void gsb_punct(Word *w, const char *t);
extern void gsb_set_positions(Word *w);
extern void gsb_show(FILE *tab, Trun *r, int with_form);
extern void gsb_sign(Trun *r, const char *t);
extern void gsb_value(Trun *r, const char *t);

extern void gsig_print(FILE *fp, Gsig *gp, const char *id_sig_sep);

#endif/*TOK_H_*/
