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
  char buf[strlen(oracc())+strlen("/osl/")+strlen(script)+strlen("/02pub/.oss0")];
  sprintf(buf, "%s/osl/02pub/%s.oss", oracc(), script);
  *fnamep = (char*)pool_copy((uchar *)buf, sl->p);
  return fopen(*fnamep, "w");
}

char *
sx_script_merge_fn(struct sl_signlist *sl, const char *script)
{
  char buf[strlen(oracc())+strlen("/osl/")+strlen(script)+strlen("/02pub/.mrg0")];
  sprintf(buf, "%s/osl/02pub/%s.mrg", oracc(), script);
  return (char*)pool_copy((uchar *)buf, sl->p);
}

static FILE *
sx_merge_file(struct sl_signlist *sl, const char *script, char **fnamep)
{
  *fnamep = sx_script_merge_fn(sl, script);
  return fopen(*fnamep, "w");
}

static void
sx_analyze_script_data(List *codes, int *ossp, int *mergep)
{
  struct sl_scriptdata *sdp;
  int oss_data = 0;
  int merge_data = 0;
  for (sdp = list_first(codes); sdp; sdp = list_next(codes))
    {
      if (sdp->sset || sdp->cvnn || sdp->salt || sdp->oivs)
	++oss_data;
      if (sdp->merge)
	++merge_data;
    }
  if (ossp)
    *ossp = oss_data;
  if (mergep)
    *mergep = merge_data;
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
	  int oss_data, merge_data;
	  sx_analyze_script_data(defp->codes, &oss_data, &merge_data);
	  if (oss_data)
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
			  if (sdp->cvnn)
			    fprintf(fp, "%s\t", sdp->cvnn);
			  else
			    fputc('\t', fp);
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
	  if (merge_data)
	    {
	      FILE *fp;
	      char *fname;
	      if (stdo)
		fp = stderr;
	      else
		fp = sx_merge_file(sl, k[i], &fname);
	      if (fp)
		{
		  Hash *m = hash_create(100);
		  Hash *o = hash_create(100);
		  for (sdp = list_first(defp->codes); sdp; sdp = list_next(defp->codes))
		    {
		      if (sdp->merge)
			{
			  List *ml = hash_find(m, (uccp)sdp->merge);
			  if (!ml)
			    hash_add(m, (uccp)sdp->merge, (ml = list_create(LIST_SINGLE)));
			  list_add(ml, (void*)sdp->sign->name);
			  List *ol = hash_find(o, (uccp)sdp->merge);
			  if (!ol)
			    {
			      hash_add(o, (uccp)sdp->merge, (ol = list_create(LIST_SINGLE)));
			      struct sl_sign *ms = hash_find(sl->hsentry, (uccp)sdp->merge);
			      list_add(ol, (void*)ms->oid);
			    }
			  list_add(ol, (void*)sdp->sign->oid);
			}
		    }
		  const char **l = hash_keys(m);
		  int i;
		  for (i = 0; l[i]; ++i)
		    {
		      fprintf(fp, "%s\t", l[i]);
		      List *ml = hash_find(m, (uccp)l[i]);
		      int j = 0;
		      const char *s;
		      for (s = list_first(ml); s; s = list_next(ml))
			{
			  if (j++)
			    fputc(' ', fp);
			  fputs(s, fp);
			}
		      fputc('\n', fp);
		      /* Now the OID version of the merge */
		      List *ol = hash_find(o, (uccp)l[i]);
		      fprintf(fp, "%s++\t", (char*)list_first(ol));
		      j = 0;
		      for (s = list_first(ol); s; s = list_next(ol))
			{
			  if (j++)
			    fputc(' ', fp);
			  fputs(s, fp);
			}
		      fputc('\n', fp);
		    }
		  fclose(fp);
		}
	      else
		{
		  fprintf(stderr, "sx: unable to open %s to write merge data: %s.\n", fname, strerror(errno));
		  goto cleanup;
		}
	    }
	}
    cleanup:
      free(k);
    }
}
