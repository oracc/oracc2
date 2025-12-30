#include <oraccsys.h>
#include <roco.h>
#include "cx.h"

/* Hooks for Roco library to generate backward-compatible XMD */

static Cx *cp;

static void
cx_roco_outer(Roco *r, int i, FILE *fp)
{
  char *dest = expand(cp->project, (ccp)r->rows[i][0], "xmd");
  fprintf(fp, "<?destfile %s?>\n", dest);
}

static void
cx_roco_o(Roco *r, int i, FILE *fp)
{
  fputs("<cat>", fp);
}

static void
cx_roco_c(Roco *r, int i, FILE *fp)
{
  fputs("</cat>", fp);
}

void
cx_roco(Cx *c)
{
  cp = c;
  roco_xmd_ns = 1;
  roco_xml_row_hooks(cx_roco_outer, cx_roco_o, cx_roco_c);
}
