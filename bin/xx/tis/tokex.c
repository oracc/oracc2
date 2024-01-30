#include <oraccsys.h>
#include <vido.h>

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
  char buf[1024], *b, wdid[512];
  Vido *vp = vido_init('t', 0);
  while ((b = fgets(buf, 1024, stdin)))
    {
      if (b[strlen(b)-1] == '\n')
	b[strlen(b)-1] = '\0';

      if ('W' == *b)
	strcpy(wdid, 1+strchr(b, '\t'));
      else if ('g' == *b)
	{
	  char *t, *s, *w;
	  t = strchr(b, '\t');
	  if (t)		/* token */
	    *t++ = '\0';
	  if ('.' == *t)
	    continue;	  
	  s = strchr(t, '\t');  /* signature */
	  if (s)
	    *s++ = '\0';
	  w = strchr(s, '\t');
	  if (w)
	    *w++ = '\0';
	  printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), wdid);
	  
	  /* if the grapheme token has a part-3 (value) determine the
	   * parent and generate a token entry for it:
	   *
	   * if part-2 (form) is non-empty, the parent is part-1.part-2.
	   * if part-2 is empty, the parent is part-1..
	   *
	   * This means we can simply null-out the value and use the
	   * remainder as the parent.
	   */
	  char *v = strrchr(t, '.');
	  if (v[1])
	    {
	      char save = v[1];
	      v[1] = '\0';
	      printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), wdid);
	      v[1] = save;
	    }
	}
    }
  vido_dump_data(vp, "tid.vid", "tid.tsv");
  vido_term(vp);
}
