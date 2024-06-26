#ifndef DBXLIB_H_
#define DBXLIB_H_
#include <oraccsys.h>
#include <dbi.h>
extern const char *dbidir, *dbiname;
extern Dbi_index *wx_dbx_proj_oid(const char *project);
extern Dbi_index *wx_dbx_init(const char *dir, const char *name);
extern void wx_dbx_term(Dbi_index *dbi);
extern const char *wx_dbx_key(Dbi_index *dbi, const char *key);
#endif/*DBXLIB_H_*/
