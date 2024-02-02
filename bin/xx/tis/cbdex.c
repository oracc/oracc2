#include <oraccsys.h>
#include <tok.h>

/**tokex reads and expands the output of tokx, simplifying it to a
 * stream of triples of the form:
 *
 * token token-ID word-ID
 *
 * The expansions vary according to token type.
 *
 * For 'g' type, graphemes, the expansion includes generating parent
 * entries for values and various features based on the grapheme
 * signature.
 */

int
main(int argc, char **argv)
{
  char buf[1024], *b;
  struct cbdex_header *chp = cbdex_init();
  struct cbdex *cp = cbdex_new(chp);
  while ((b = fgets(buf, 1024, stdin)))
    {
      if (b[strlen(b)-1] == '\n')
	b[strlen(b)-1] = '\0';

      if ('A' == *b)
	{
	  cbdex_reset(cp);
	  cbdex_loc(b, cp);
	}
      else if ('B' == *b)
	{
	  cbdex_loc(b, cp);
	}
      else if ('g' == *b)
	{
	  cbdex_g(b, cp);
	  cbdex_write(stdout, cp);
	}
    }
  cbdex_term(chp);
}
