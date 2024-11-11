#include <oraccsys.h>

/**file_args: set up input and output variables based on a qualified
 * PQX ID and the calling environment.
 *
 * If outext is set, construct an output file name, possible including trans.
 *
 * If htmldir is non-null it must be the P4 html cache directory; we
 * construct an output path suitable for use by P4.
 *
 */

int
file_args(const char *htmldir, const char *qpqx, const char *inext,
	  const char *outfile, const char *outext, const char *trans,
	  char **inp, char **outp, char **hdir)
{
  expand_base(NULL);
  *inp = strdup(expand(NULL, qpqx, inext));

  if (!outfile)
    {
      if (htmldir)
	{
	  expand_base(htmldir);
	  *hdir = strdup(expand(NULL, qpqx, NULL));
	}
      char *ex = expand(NULL, qpqx, outext);
      if (trans && strcmp(trans, "en"))
	{
	  char *out = malloc(strlen(ex)+strlen(trans)+2);
	  sprintf((char*)out, "%s.%s", ex, trans);
	  *outp = out;
	}
      else
	*outp = strdup(ex);
    }
  else
    *outp = strdup(outfile);

  return 0;
}
