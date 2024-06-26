#ifndef DBXLIB_H_
#define DBXLIB_H_
#include <dbi.h>

extern char dbi_sep_char;
extern Dbi_index *dbx_init(const char *dir, const char *name);
extern void dbx_term(Dbi_index *dbi);
extern const void *dbx_key(Dbi_index *dbi, const char *key, Unsigned32 *np);
extern void dbx_vido(Dbi_index *dp);
extern void dbx_wids(Dbi_index *dp, const Loc8 *l8p, int n, FILE *o);
extern void dbx_l8s(Dbi_index *dp, Loc8 *l8p, int n, FILE *o);
extern int dbx_access(const char *dir, const char *name);
extern const void *dbx_one_off(const char *dir, const char *name, const char *key, Unsigned32 *n);

#endif/*DBXLIB_H_*/
