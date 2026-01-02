#ifndef XML2TSV_H_
#define XML2TSV_H_

#include <roco.h>
#include "xmlns_lib.h"

extern int fmp_mode, nfld, xml_mode, verbose;
extern size_t nrec;
extern const char **fields;
extern Roco *r;

extern void x2t_load_data(const char *fn, Roco *r);
extern void x2t_set_fields(const char *fn);

#endif/*XML2TSV_H_*/
