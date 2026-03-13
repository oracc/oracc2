#ifndef LONGTEXT_H_
#define LONGTEXT_H_
extern char *longtext(Pool *p, char *t, char *a);
extern void longtext_init(Pool *p, char *s);
extern char *longtext_sep(Pool *p, char *t, char *a, const char *sep);
extern void longtext_set_sep(const char *s);
#define longtext_get() (unsigned char*)longtext(NULL,NULL,NULL)
#endif/*LONGTEXT_H_*/
