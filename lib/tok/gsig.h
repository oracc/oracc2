#ifndef GSIG_H_
#define GSIG_H_

typedef struct gsig
{
  char gdltype;
  char role; 		/* d(eterminative-ante)|D(eterminative-post) g(loss) p(unctuation) w(ord-constituent) u(ndetermined) */
  char roletype;	/* role=d: s(emantic) p(honetic) v(ariant) c(uneiform[i.e.paleographic]) u(ndetermined)
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
  char position;      	/* Position letter codes are contrived to sort in the way signlists list words:
			   a(absolute)
			   i(nitial)
			   m(edial)
			   t(erminal)
 			   u(ndetermined)
			   - (ignore) -- used for determinatives in no_d_position
 			 */
  char no_d_position;
  char c_position;
  char preserved;	/* + = whole; . = half-brackets; - = square brackets */
  char editorial;	/* n = normal; e = excised; i = implied; m = maybe; s = supplied; r = erased; v = some */
  char* flags;		/* + = none; ! * ? = as ATF ; 1 2 3 4 = as user flags */
  char *sig;	/* The rendered sig, NULL for XTF data but used when reading .tok data */
  char *coresig;  /* Just the SIGN-FORM-VALUE part of ths sig */
  char *project; 	/* project that owns the instance; xxx for instances that do no come from a text */
  char *asltype; 	/* pc pe sl */
  char *form;		/* g:X form, value, sign, compound etc. */
  char *lang;		/* primary language of word */
  char *logolang;	/* secondary language for logogram */
  char *soid;		/* sign-oid */
  char *sname;		/* sign-name */
  char *seq;		/* sign-sequence-oids */
  char *foid;		/* form-oid */
  char *fname;		/* form-name */
  char *value;		/* value when gdltype == 'v' */
  int index;		/* Index of grapheme in word, counting from 1; 0 = no g:w parent */
  int last;		/* 1 = final grapheme in word */
  int no_d_index;	/* Index ignoring initial determinatives */
  int no_d_last;	/* 1 = final grapheme in word ignoring initial/final determinatives */
  int c_index;		/* Index of c in word; c-elements move index but not c_index */
  int ce_index;		/* Index of c-element in compound */
  struct wloc *w;
} Gsig;


extern char *gsig_index(const char *gs);

#endif/*GSIG_H_*/
