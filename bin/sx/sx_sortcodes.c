#include <ctype128.h>
#include <oraccsys.h>
#include <signlist.h>
#include <sx.h>

void
sx_sortcodes(struct sl_signlist *sl)
{
  if (sl && sl->tokens)
    {
      int i;
      for (i = 0; sl->tokens[i]; ++i)
	printf("%s\t%d\n", sl->tokens[i]->t, sl->tokens[i]->s);
    }
}
