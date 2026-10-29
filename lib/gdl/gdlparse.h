#ifndef GDLPARSE_H_
#define GDLPARSE_H_
extern Tree *gdlparse_string(Mloc *m, char *s);
extern void gdlparse_init(void);
extern void gdlparse_reset(void);
extern void gdlparse_term(void);
#endif/*GDLPARSE_H*/
