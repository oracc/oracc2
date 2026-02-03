#ifndef ORACCSYS_H_
#define ORACCSYS_H_

#include <locale.h>
#include <oracclocale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wchar.h>
#include <wctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <glob.h>
#include <c2types.h>
#include <hash.h>
#include <keva.h>
#include <pool.h>
#include <loadfile.h>
#include <ctype128.h>
#include <vector.h>
#include <ose.h>
#include <dbxlib.h>
#include <xsystem.h>
#include <bits.h>

#define QUALIFIED_PQX_MAX	512

#define TIS_TEMPLATE "t123456"

typedef int sort_cmp_func(const void*,const void*);

extern int ff_verbose;

/* Defines for porting L2 and other old code */
#define xaccess(buf,mode,quit) access(buf,mode)
#define vwarning2 mesg_vwarning
#define vwarning2 mesg_vwarning
#define	default_ext(fn,fext) new_ext(fn, fext, FALSE)
#define	force_ext(fn,fext) new_ext(fn, fext, TRUE)
#define	default_path(fn,fpath) new_path(fn, fpath, FALSE)
#define	force_path(fn,fpath) new_path(fn, fpath, TRUE)

extern unsigned char *slurp(const char *caller, const char *fname, ssize_t *fsizep);

extern void expand_base(const char *d);
extern  char *expand(const char *project,const char *pqid,const char *ext);
extern  char *expand_xtr (const char *project, const char *pqid, const char *code, const char *lang);
extern  char * find_ext(char *fn);
extern  char * find_path(char *fn);
extern  char * basename(const char *fn);
extern  char * extname(char *fn);
extern  char * filename(char *fn);
extern  char * pathname(char *fn);
extern  char * new_ext(char *fn, const char *fext, int force);
extern  char * new_ext_no_alloc(char *oldfile,const char *ext);
extern  char * new_path(char *fn, const char *fpath, int force);
extern const char *oracc (void);
extern const char *oracc_builds (void);
extern char *oracc_data(const char *fname);
extern const char *oracc_home (void);
extern const char *oracc_project_home (void);
extern char *oracc_rnc(const char *fname);
extern const char *oracc_var (void);
extern const char *projectify(const char *path, const char **projp);

extern void u_upper(unsigned char *s, int n);
extern int u_charbytes(const unsigned char *g);
extern const unsigned char *utf_lcase(const unsigned char *s);
extern const unsigned char *utf_ucase(const unsigned char *s);
extern wchar_t utf1char(const unsigned char *src,size_t *len);
extern unsigned char *utf2atf(const unsigned char *src);
extern wchar_t* utf2wcs(const unsigned char *src, size_t *len);
extern int u_isupper(const unsigned char *g);
extern int u_islower(const unsigned char *g);
extern unsigned char *wcs2atf(wchar_t *wc, size_t len);
extern unsigned char *wcs2utf(const wchar_t*ws, size_t len);
extern wchar_t*wcs_lcase(wchar_t*ws);
extern wchar_t*wcs_ucase(wchar_t*ws);
extern wchar_t*wcs_undet(wchar_t*ws);
extern unsigned char *sexify(int n, const char *aszdisz);
extern unsigned char *utf8ify(unsigned int c);

extern int cmpstringp(const void *p1, const void *p2);

extern uint32_t hex2int(char *hex);
extern unsigned char *uhex2utf8(unsigned const char *uhex);

extern void help (void);
extern int opts (int optchar, const char *optarg);
extern int options (int argc, char *const *argv, const char *optstr);
extern void banner(void);
extern void usage(void);
extern int optind, opterr;
extern char *optarg;
extern void program_values(const char *name, int major, int minor, const char *usage, const char *copyright);
extern void strrep_f_h(const char *str, FILE *fp, Hash*reps);

extern const char *itoa(int i);

extern char **dot_split(char *l);
extern char **space_split(char *l);
extern char **tab_split(char *l);

extern int fcheckf (const char *fn1, const char *fn2);
extern int fcheckf_list (char *fn1, char **fn2s);
extern off_t fsize (const char *fn, FILE *fp);

extern int file_copy(const char *from, const char *to);

extern int xmkdir (const char *path, mode_t mode, Boolean quit);

extern int glob_pattern(const char *pat, glob_t *gres);
extern int pctint(int amount, int total);
extern double pctd(double amount, double total);
extern char *cfy_fam(const char *project);

extern char *projhyph(const char *p);

extern char** dirarray(const char* path, int* count);
extern char *findfile(const char *project, const char *file2find, const char *deflt);
extern char *findfile_dir(const char *project, const char *file2find, const char *deflt, const char *dir);
extern int rnv_check(const char *rnc, const char *xml);
extern char *normalize_ws(const char *s);
extern int xmkdirs(const char *dir);

extern const char *opts_prog(void);

#endif/*ORACCSYS_H_*/
