#include <oraccsys.h>
#include "vx.h"

extern int s_words;
#define ucun_x "\xE2\x96\x92"

void
vx_tra_node(Node *np, FILE *fp)
{
  if (np->text)
    fputs(np->text, fp);
}
