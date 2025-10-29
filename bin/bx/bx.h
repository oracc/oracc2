#ifndef BX_H_
#define BX_H_

extern const char *bblfile;
extern Pool *p;
extern const char *project;
extern int verbose;

extern const char **bx_bibs_file(const char *fn);
extern void bx_bibs_res(const char *project, const char **bibfiles);
extern void bx_keys(const char *project, const char **bibfiles);

#endif/*BX_H_*/
