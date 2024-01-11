#ifndef GSIG_H_
#define GSIG_H_

/* This should be allocated for each input */
struct gsb_input
{
  struct gsb_word *wpp;
  const char *projproj;
  const char *textproj;
  int wpp_alloced;
  int wpp_used;
  int in_c;
  int in_n;
  int in_p;
  int in_q;
  Pool *p;
};

/* This should be allocated for each sH/g:w; for line-oriented
   programs it can be reset rather than reallocated.
 */
struct gsb_word
{
  struct gsig *gpp;
  struct gsig *curr_c_wgp;
  const char *lang;
  const char *form;
  char role;			/* transient holders for role/roletext
				   that go with child graphemes */
  const char *roletext;
  int gpp_alloced;
  int gpp_used;
  int no_d_index;
  struct gsb_input *in;
};

typedef struct gsig
{
  char gdltype;
  char role; 		/* d(eterminative) g(loss) p(unctuation) w(ord-constituent) u(ndetermined) */
  char roletype;	/* role=d: s(emantic) p(honetic) v(ariant) u(ndetermined)
			   role=g: t(ranslation) v(ariant)
			   role=p: b(ullet=*) d(ivider-general=:-etc) w(ord-divider) s(urrogate) u(ndetermined)
			   role=w|u: n(one)
			 */
  char type;		/* i(deogram)
			   m(orpheme) M(ixed-morpho-ideo/syll)
			   l(ogogram[secondary-language])
			   s(yllable)
			   u(ndetermined)
			   c(compound-element)
			 */
  char position;	/* i(ndependent) b(eginning) m(iddle) e(nd) u(ndetermined) */
  char no_d_position;   /* i(ndependent) b(eginning) m(iddle) e(nd) u(ndetermined) */
  char c_position;   	/* for type=c: position in compound: i(ndependent) b(eginning) m(iddle) e(nd) u(ndetermined) */
  const char *project; 	/* project that owns the instance; xxx for instances that do no come from a text */
  char *asltype; 	/* pc pe sl */
  char *form;		/* g:X form, value, sign, compound etc. */
  char *lang;		/* primary language of word */
  char *logolang;	/* secondary language for logogram */
  char *soid;		/* sign-oid */
  char *sname;		/* sign-name */
  char *foid;		/* form-oid */
  char *fname;		/* form-name */
  char *value;		/* value when gdltype == 'v' */
  int index;		/* Index of grapheme in word, counting from 1; 0 = no g:w parent */
  int last;		/* 1 = final grapheme in word */
  int no_d_index;	/* Index ignoring initial determinatives */
  int no_d_last;	/* 1 = final grapheme in word ignoring final determinatives */
  int c_index;		/* Index of c in word; c-elements move index but not c_index */
  int ce_index;		/* Index of c-element in compound */
  struct gsb_word *w;
} Gsig;

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

#endif/*GSIG_H_*/
