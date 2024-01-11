#ifndef TOK_H_
#define TOK_H_

#include <trun.h>
#include <loch.h>
#include <gsig.h>

extern void gsig_print(FILE *fp, Gsig *gp, const char *id_sig_sep);
extern Gsig* gsig_parse(char *s, Gsig *gp, const char *id_sig_sep);

extern struct gsb_input *gsb_input_init(void);
extern void gsb_input_term(struct gsb_input *gsip);
extern struct gsb_word *gsb_word_init(struct gsb_input *gsip);
extern void gsb_word_reset(struct gsb_word *gswp);
extern void gsb_word_term(struct gsb_word *gswp);
extern Gsig *gsb_new(struct gsb_word *gswp);
extern Gsig *gsb_get(struct gsb_word *gswp);
extern Gsig *gsb_get_n(struct gsb_word *gswp, int n);
extern void gsb_add(struct gsb_word *gswp, char type, const char *form, const char *oid,
		    const char *sign, const char *spoid, const char *spsign, const char *lang, 
		    const char *logolang);
extern void gsb_set_positions(struct gsb_word *gswp);
extern void gsb_last(struct gsb_word *gswp);
extern void gsb_c_last(struct gsb_word *gswp);
extern void gsb_punct(struct gsb_word *gswp, const char *t);
extern void gsb_sign(struct gsb_word *gswp, const char *t);
extern void gsb_value(struct gsb_word *gswp, const char *t);
extern void gsb_show(FILE *tab, struct gsb_word *gswp, int with_form);
extern void gsb_reset_word(struct gsb_word *gswp);


#endif/*TOK_H_*/
