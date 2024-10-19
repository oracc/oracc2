#include <oraccsys.h>
#include <vido.h>

/**tokex reads and expands the output of tokx, simplifying it to a
 * stream of triples of the form:
 *
 * token token-ID word-ID
 *
 * The expansions vary according to token type.
 *
 * For 'g' type, graphemes, the expansion includes generating SIGN and
 * FORM parent entries for values and various features based on the
 * grapheme signature.
 */

const char *index_dir = "02pub/tok";
int no_output = 0;

int
main(int argc, char **argv)
{
  options(argc,argv,"d:n");
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

	  /* Always save the entire S.F.V */
	  printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);

	  /* if the key contains ..[^\000] the SFV is o0..v so just zero dot[2] and save the sign key */
	  char *dot = strchr(t, '.');
	  if ('.' == dot[1])
	    {
	      if (dot[2])
		{
		  dot[2] = '\0';
		  printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);
		}
	    }
	  else
	    {
	      /* we have to save S.F. */
	      dot = strrchr(t, '.');
	      if (dot[2])
		{
		  dot[1] = '\0';
		  printf("%s\t%s\t%s\n", t, vido_new_id(vp,t), qid);
		}
	    }
	}
    }
  if (!no_output)
    {
      char vidfile[strlen(index_dir)+strlen("tid.vid0")];
      char tsvfile[strlen(index_dir)+strlen("tid.vid0")];
      sprintf(vidfile,"%s/tid.vid",index_dir);
      sprintf(tsvfile,"%s/tid.tsv",index_dir);
      vido_dump_data(vp, vidfile, tsvfile);
      vido_term(vp);
    }
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'd':
      index_dir = arg;
      break;
    case 'n':
      no_output = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "tokex";
int major_version = 6, minor_version = 0, verbose;
const char *usage_string = "-d [index_dir] [-n]";
void
help ()
{
  printf("  -d [index_dir] Gives the name of the index directory; defaults to 02pub/tok\n");
  printf("  -n no output; suppress vid dump\n");
}
