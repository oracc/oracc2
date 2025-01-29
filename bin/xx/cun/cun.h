#ifndef CUN_H_
#define CUN_H_

#define CT_NOT 0
#define CT_ELL 0x01
#define CT_XXX 0x02
#define CT_GRP 0x04
struct d
{
  int type;
  const char *name;
  const char *oid;
  const char *utf8;
  const char *form;
  const char *xid; /* xml:id */
  int gsp;
  int wsp;
  int brk; /* broken */
  int lig; /* flag for dp is the result of ligature gathering */
  int mis; /* only used when lig==1 to indicate all components of lig had g:break=="missing" */
#if 1
  int gmissing;
#else
  Hash *atts;
#endif
};

extern Pool *p;
extern Hash *lig;
extern List *cqueue;
extern FILE *outfp;

struct perfnt
{
  const char *name;
  const char *fnt;
};
extern struct perfnt *perfnt (register const char *str, register size_t len);

extern void cfy_render(void);
extern int file_args(const char *htmldir, const char *qpqx, const char *inext,
		     const char *outdir, const char *outext, const char *trans,
		     char **inp, char **outp, char **hdir);

#endif/*CUN_H_*/
