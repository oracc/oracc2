#include <oraccsys.h>
#include <lng.h>
#include "sigs.h"
#include "ilem.h"
#include "form.h"
#include "xcl.h"

extern const char *textid;

static void sigs_d_show_insts(struct xcl_context *xcp, struct xcl_d *d);
static void sigs_l_show_insts(struct xcl_context *xcp, struct xcl_l *l);

void
sig_show_insts(struct xcl_context *xcp)
{
  xcl_map(xcp, NULL, NULL, sigs_d_show_insts, sigs_l_show_insts);
}

static void
sigs_d_show_insts(struct xcl_context *xcp, struct xcl_d *d)
{
  if (sparse_lem)
    {
      if (d->type == xcl_d_field_start)
	xcl_set_sparse_skipping(d->subtype);
      else if (d->type == xcl_d_field_end)
	xcl_set_sparse_skipping(NULL);
    }
}

static void
sigs_l_show_insts(struct xcl_context *xcp, struct xcl_l *l)
{
  extern const char *project;
  if (sparse_skipping)
    {
      BIT_SET(l->f->instance_flags, ILEM_FLAG_SPARSE_SKIP);
      return;
    }
  /*  if (l && l->f && l->f->sublem) */
  if (l)
    {
      printf("%s:%d: ", l->f->file, (int)l->f->lnum);
      printf("%s", textid);
      if (l->f && l->f->sublem)
	printf("@%s%s\n", project, l->f->sublem);
      else if (l->inst)
	printf("@%s%s\n", project, l->inst);
    }
}
