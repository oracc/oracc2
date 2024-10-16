#include <oraccsys.h>
#include <signlist.h>

static unsigned char *
sx_script_ivs(struct sl_scriptdata *sdp)
{
  unsigned int ivs = (unsigned int)strtoul(sdp->oivs, NULL, 16);
  return utf8ify(ivs);
}

static FILE *
sx_script_file(struct sl_signlist *sl, const char *script, char **fnamep)
{
  char buf[strlen(script)+strlen("/02pub/.oss0")];
  sprintf(buf, "02pub/%s.oss", script);
  *fnamep = (char*)pool_copy((uchar *)buf, sl->p);
  return fopen(*fnamep, "w");
}

static int
sx_has_oss_data(List *codes)
{
  struct sl_scriptdata *sdp;
  for (sdp = list_first(codes); sdp; sdp = list_next(codes))
    if (sdp->sset || sdp->salt || sdp->oivs)
      return 1;
  return 0;
}

void
sx_script(struct sl_signlist *sl, int stdo)
{
  if (sl->h_scripts->key_count)
    {
      const char **k = hash_keys(sl->h_scripts);
      int i;
      for (i = 0; k[i]; ++i)
	{
	  struct sl_scriptdef *defp = hash_find(sl->h_scripts,(uccp) k[i]);
	  struct sl_scriptdata *sdp;
	  if (sx_has_oss_data(defp->codes))
	    {
	      FILE *fp;
	      char *fname;
	      if (stdo)
		fp = stderr;
	      else
		fp = sx_script_file(sl, k[i], &fname);
	      if (fp)
		{
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
		  fclose(fp);
		}
	      else
		{
		  fprintf(stderr, "sx: unable to open %s to write script data: %s.\n", fname, strerror(errno));
		  goto cleanup;
		}
	    }
	}
    cleanup:
      free(k);
    }
}
