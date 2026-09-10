#include <oraccsys.h>
#include "vx.h"

static void
vxc_header(Tree *tp, FILE *fp)
{
}

static void
vxc_sentences(Tree *tp, FILE *fp)
{
  Node *xcl = vx_component(tp, "xcl:xcl");
  if (xcl)
    {
      List *s = vx_tags_by_attr(xcl, "xcl:c", "type", "sentence", 0);
      fprintf(stderr, "found %ld sentences\n", list_len(s));
    }
}

void
vx_conllo(Tree *tp, FILE *fp)
{
  vxc_header(tp, fp);
  vxc_sentences(tp, fp);
}
