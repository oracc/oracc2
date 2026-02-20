#ifndef ASL_H_
#define ASL_H_ 1

#include <tree.h>

#include "signlist.h"

extern struct sl_signlist *curr_asl;
extern int asl_raw_tokens;
extern int aslflextrace;

extern const char *curraslfile, *aslfile;

extern int asl_at_check(const char *atp);
extern struct sl_signlist *aslyacc(const char *file);
extern void asl_init(struct sl_config *cp);
extern void asl_term(void);
extern int aslparse(void);
extern Hash *asl_get_oids(void);
extern struct sl_config *asl_get_config(void);

#endif
