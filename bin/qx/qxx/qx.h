#ifndef QX_H_
#define QX_H_

#include <types.h>
#include <se.h>
#include <selib.h>

struct qxdata
{
  const char **argv;	/* argv is original argv from main() ... */
  int argc;		/* ... but argc is index of first search token in argv */
  int any_index;
  int debug;
  int noexec;
  int web;
  const char *project;
  const char *glos;
  const char *srch;
  const char *cemd; /* passed through to px */
  const char *bar; /* the search bar is written possibly adding cbd/lang index and stored here */
  const char **srchv;
  const char *err;
  FILE *listfp;
  Pool *p;
};

extern struct qxdata qd;

struct sdata
{
  const char *sub; /* equivalent to ip->cache.sub */
  const char *prj;
  const char *pub;
  const char *tmp;
  const char *bar;
  const char *lst;
  const char *idx;
  const char *dir; /* idx dir */
  const char *nam; /* idx nam */
  const char *dbi;
  const char *err;
  int count;
  const char *gran;
  Pool *p;
};

extern struct sdata sdata;

extern FILE *f_log;
extern int any_index;
extern int do_uniq;
extern int doing_debug;
extern int l2;
extern int p4;
extern int s2;
extern int show_count;
extern int show_tokens;
extern int textresult;
extern int verbose;
extern const char *xmldir;
extern const char *errfile;
extern const char *outfile;
extern const char *arg_index;
extern const char *project;
extern const char *return_index;
extern FILE*out_f;
extern FILE*f_err;
extern FILE *f_log;
extern enum result_granularity res_gran, best_res_gran;
extern char *pretrim_file;
extern unsigned char **pretrim_lines, *pretrim_content;
extern List *pretrim_args;
extern Hash *pretrim;
extern FILE *f_mangletab;

extern const char *se_curr_project, *curr_index, *cbd_lang, *textid;

#include "all.proto"

#endif/*QX_H_*/
