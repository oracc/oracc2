#ifndef GSIG_H_
#define GSIG_H_

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
  struct wloc *w;
} Gsig;

#endif/*GSIG_H_*/
