#ifndef ATFDATA_H_
#define ATFDATA_H_
extern int split_dups, fix_dups_mode;
extern FILE *keepfp, *outfp, *splitfp;
extern void atf_fix_dup(char *pqx);
extern FILE *atfi_input(const char *fn);
extern FILE *atfi_output(const char *fn, const char *odir);
extern FILE *atfi_split(const char *fn, const char *odir);
#endif/*ATFDATA_H*/
