#ifndef GDLPARSE_H_
#define GDLPARSE_H_
extern void gdlparse_string(Mloc *m, char *s);
extern char *gdl_one_off(const char *file, int line, const char *g, int voidsl);
extern void gdlparse_init(void);
extern void gdlparse_reset(void);
extern void gdlparse_term(void);
#endif/*GDLPARSE_H*/
