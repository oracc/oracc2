#ifndef DBXLIB_H_
#define DBXLIB_H_
#include <dbi.h>

extern const char *dbidir, *dbiname;
extern Dbi_index *dbx_init(const char *dir, const char *name);
extern void dbx_term(Dbi_index *dbi);
extern const void *dbx_key(Dbi_index *dbi, const char *key, Unsigned32 *np);
extern void dbx_vido(Dbi_index *dp);

#endif/*DBXLIB_H_*/
