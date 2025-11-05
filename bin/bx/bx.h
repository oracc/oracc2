#ifndef BX_H_
#define BX_H_

#include <hash.h>
#include <pool.h>
#include <list.h>
#include <memo.h>
struct bx;

typedef enum bxmode { BX_CIT , BX_ICF , BX_KEY , BX_REF, BX_TOP } Bxmode;

#define BX_DB_PEOPLE 1
#define BX_HTML_DIV  1
#define BX_HTML_PAGE 2

typedef void (*bxfunc)(struct bx*);

typedef struct bx
{
  Bxmode mode;
  char * const*argv;
  Hash *keys;
  Hash *keys_cit;
  Hash *names;
  Hash *akanames;
  Hash *hlasts;
  Hash *refs;
  Memo *m_bib;
  Memo *m_bibentry;
  Memo *m_bibfield;
  Memo *m_bibicf;
  Memo *m_name;
  Memo *m_name_ptr;
  Pool *p;
  List *mem; /* odd memory items that need to be freed at end of run */
  const char *outfile;
  const char *project;
  const char *bibkey_file;
  Hash *hbibkey;
  Hash *hicf;
  const char *flist_bib;
  const char **files_bib;
  const char *flist_cit;
  const char **files_cit;
  const char *citations_file;
  const char **citations;
  List *bibs;
  List *entries;	/* used for !bibonly run; redirects to
			   per-bib-file entries in bibonly mode */
  struct bibentry **ents;
  int nents;
  struct name **people;
  int npeople;
  bxfunc pre[BX_TOP];
  bxfunc run[BX_TOP];
  bxfunc out[BX_TOP];
  FILE *outfp; /* current output for icf mode */
  int bibonly;
  int dbs;
  int html_mode;
  int icfonly;
  int no_output;
  int quiet;
  int sort;
  int xmloutput;
} Bx;

extern Bx bx;

#define m_cit(b) ((b)->mode==BX_CIT)
#define m_icf(b) ((b)->mode==BX_ICF)
#define m_key(b) ((b)->mode==BX_KEY)
#define m_ref(b) ((b)->mode==BX_REF)

extern int verbose;

extern const char **bx_bibs_file(const char *fn);
extern void bx_keys(Bx *bp, const char *project, const char **bibfiles);

extern void bx_cit_pre(Bx *bp);
extern void bx_icf_pre(Bx *bp);
extern void bx_key_pre(Bx *bp);
extern void bx_ref_pre(Bx *bp);

extern void bx_cit_run(Bx *bp);
extern void bx_icf_run(Bx *bp);
extern void bx_key_run(Bx *bp);
extern void bx_ref_run(Bx *bp);

extern void bx_cit_out(Bx *bp);
extern void bx_icf_out(Bx *bp);
extern void bx_key_out(Bx *bp);
extern void bx_ref_out(Bx *bp);

extern void bxl_bibkeys(Bx *bp);
extern void bxl_bibkey_file(Bx *bp);
extern const char **bxl_flist_files(Bx *bp, const char *flist, const char *ext, char **fmem);
extern void bxl_key_writer(Bx *bp, FILE *fp);
extern void bxl_bib_files(Bx *bp);

#endif/*BX_H_*/
