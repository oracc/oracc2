#ifndef BX_H_
#define BX_H_

struct bx;

typedef enum bxmode { BX_CIT , BX_ICF , BX_KEY , BX_REF, BX_TOP } Bxmode;

typedef void (*bxfunc)(struct bx*);

typedef struct bx
{
  Bxmode mode;
  Hash *keys;
  Hash *refs;
  Pool *p;
  const char *project;
  bxfunc pre[BX_TOP];
  bxfunc run[BX_TOP];
  bxfunc out[BX_TOP];
} Bx;

extern Bx bx;

#define m_cit (b.mode==BX_cit)
#define m_icf (b.mode==BX_icf)
#define m_key (b.mode==BX_key)
#define m_ref (b.mode==BX_ref)

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

#endif/*BX_H_*/
