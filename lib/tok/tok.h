#ifndef TOK_H_
#define TOK_H_

#include <trun.h>
#include <loch.h>
#include <gsig.h>

extern Trun *trun_init(int multi);
extern void trun_term(Trun *r);
extern Word *trun_word_init(Trun *r);
extern void trun_word_reset(Word *w);
extern void trun_word_term(Word *w);

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

extern Loch *tlb_init(Trun *r, const char *project, const char *type);
extern void tlb_term(Trun *r);
extern void tlb_T(Trun *r, const char *p, const char *id, const char *n);
extern void tlb_L(Trun *r, const char *num, const char *id, const char *lab);
extern void tlb_W(Trun *r, const char *id, const char *lang, const char *form);
extern void tlb_K(Trun *r, const char *k, const char *v);
extern void tlb_K_wrapup(Trun *r);
extern void tlb_F(Trun *r, const char *filename);
extern void tlb_P(Trun *r, const char *project);
extern void tlb_Y(Trun *r, const char *rtype);
extern void tlb_G(Trun *r, const char *glosslang);
extern void tlb_A(Trun *r, const char *oid, const char *cgp);
extern void tlb_M(Trun *r, const char *type, const char *wids, const char *oid, const char *name);


#endif/*TOK_H_*/
