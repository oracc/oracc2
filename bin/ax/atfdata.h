#ifndef ATFDATA_H_
#define ATFDATA_H_
extern int split_dups, fix_dups_mode, dry;
extern int x_mode; /* extract ID and NAME for X-IDs */
extern FILE *keepfp, *outfp, *splitfp;
extern const char *curr_pqx;
extern const char *curr_file;
extern int curr_line;
extern void atf_fix_dup(char *pqx);
extern FILE *atfd_input(const char *fn);
extern FILE *atfi_input(const char *fn);
extern FILE *atfi_output(const char *fn, const char *odir);
extern FILE *atfi_split(const char *fn, const char *odir);
#endif/*ATFDATA_H*/
