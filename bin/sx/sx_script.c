#include <oraccsys.h>
#include <signlist.h>

unsigned char *
sx_script_ivs(struct sl_scriptdata *sdp)
{
  unsigned int ivs = (unsigned int)strtoul(sdp->oivs, NULL, 16);
  return utf8ify(ivs);
}

void
sx_script(struct sl_signlist *sl, int stdo)
{
  if (sl->h_scripts->key_count)
    {
      stdo = 1;
      const char **k = hash_keys(sl->h_scripts);
      int i;
      for (i = 0; k[i]; ++i)
	{
	  FILE *fp;
	  if (stdo)
	    fp = stderr;
	  else
	    fp = NULL; /* open file in PROJECT/02pub/k[i].oss (oracc script-style) */
	  struct sl_scriptdef *defp = hash_find(sl->h_scripts,(uccp) k[i]);
	  struct sl_scriptdata *sdp;
	  for (sdp = list_first(defp->codes); sdp; sdp = list_next(defp->codes))
	    {
	      if (sdp->sset || sdp->salt || sdp->oivs)
		{
		  fprintf(fp, "%s\t", sdp->code);
		  if (sdp->sset)
		    fprintf(fp, "%s\t", sdp->sset);
		  else
		    fputc('\t', fp); /* default is to allow enclosing script-style to set sset */
		  if (sdp->salt)
		    fprintf(fp, "%s\t", sdp->salt);
		  else
		    fputc('\t', fp);
		  if (sdp->oivs)
		    fprintf(fp, "%s%s", sdp->sign->U.utf8, sx_script_ivs(sdp));
		  fputc('\n', fp);
		}
	    }
	}
    }
}
