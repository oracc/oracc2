#ifndef GDLPARSE_H_
#define GDLPARSE_H_
extern Tree *gdlparse_string(Mloc *m, char *s);
extern void gdlparse_init(GDL *gp);
extern void gdlparse_reset(void);
extern void gdlparse_term(void);
extern void gdlparse_start(Tree *tp, GDL *gp);
extern void gdlparse_stop(void);
#endif/*GDLPARSE_H*/
