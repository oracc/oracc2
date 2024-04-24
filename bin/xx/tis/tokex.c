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

const char *index_dir = "02pub/tok";

int
main(int argc, char **argv)
{
  options(argc,argv,"d:");
  char buf[1024], *b, qid[1024], wdid[32];
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
	  if ('@' == *s)
	    {
	      ++s;
	      char *t = strchr(s, '%');
	      *t = '\0';
	      strcpy(qid, s);
	      strcat(qid, ":");
	      strcat(qid, wdid);
	    }
	  w = strchr(s, '\t');
	  if (w)
	    *w++ = '\0';

	  printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);
	  
	  /* if the grapheme token has a ter (value) determine the
	   * parent and generate a token entry for it:
	   *
	   * if bis (form) is non-empty, the parent is sem.bis.
	   * if bis is empty, the parent is sem..
	   *
	   * This means we can simply null-out the value and use the
	   * remainder as the parent.
	   *
	   * if the token has no ter we generate a token with 0 for
	   * ter so we can accurately sum instances.
	   */
	  char *v = strrchr(t, '.');
	  if (v[1])
	    {
	      char save = v[1];
	      v[1] = '\0';
	      printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);
	      v[1] = save;
	    }
	  else
	    {
	      char t0[strlen(t)+2];
	      strcpy(t0, t);
	      strcat(t0, "0");
	      printf("%s\t%s\t%s\n", t0, vido_new_id(vp,t0), qid);
	    }	    
	}
    }
  char vidfile[strlen(index_dir)+strlen("tid.vid0")];
  char tsvfile[strlen(index_dir)+strlen("tid.vid0")];
  sprintf(vidfile,"%s/tid.vid",index_dir);
  sprintf(tsvfile,"%s/tid.tsv",index_dir);
  vido_dump_data(vp, vidfile, tsvfile);
  vido_term(vp);
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'd':
      index_dir = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "tokex";
int major_version = 6, minor_version = 0, verbose;
const char *usage_string = "-d [index_dir]";
void
help ()
{
  printf("  -d [index_dir] Gives the name of the index directory; defaults to 02pub/tok\n");
}
