#include <oraccsys.h>
#include "qx.h"

extern int ntoks;
extern struct token *toks;
extern struct Datum result;
extern Vido *vp;

int
se_any(struct qxdata *qp, struct sdata *sdp)
{
  const char *xindex[] = { "!cat" , "!lem" , "!tra" , NULL, "!txt", "!esp" , NULL };
  const char *index[7] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
  Vido *vps[7];
	  
  struct Datum results[4];
  int i, best_findset = -1;
  char *hashproj = NULL;
  
#if 0  
  FILE *anyout = stdout;
  int anyout_closable = 0;
#endif
  
  const char **toklist = NULL;
  int usable = 0;

  const char **tokv = qp->srchv ? qp->srchv : (const char **)(qp->argv+qp->argc);

  if (!project)
    {
      fprintf(stderr, "se: must give -j PROJECT option when using -a\n");
      exit(1);
    }

#if 0
  if (xmldir)
    {
      char *anyfile = malloc(strlen(xmldir)+5);
      sprintf(anyfile,"%s/any",xmldir);
      anyout = xfopen(anyfile,"w");
      anyout_closable = 1;
    }
#endif
  
  hashproj = malloc(strlen(project) + 2);
  sprintf(hashproj, "#%s",project);

  for (i = 0; xindex[i]; ++i)
    {
      const char *proj_index_path = NULL;	      
      char *index_dbi = NULL;
      const char *idx = NULL;

      idx = xindex[i]+1;
      proj_index_path = se_dir(project,idx);
      index_dbi = malloc(strlen(proj_index_path) + strlen(idx) + strlen("/.dbi") + 1);
      sprintf(index_dbi, "%s/%s.dbi",proj_index_path,idx);
      if (!xaccess(index_dbi, R_OK, 0))
	{
	  /* fprintf(stderr, "adding %s\n", idx);*/
	  index[usable++] = xindex[i];
	}
    }
  index[usable] = NULL;
	  
  for (i = 0; index[i]; ++i)
    {
      if (!strcmp(index[i], "!lem"))
	{
	  if (best_findset > 0)
	    continue;
	}

      toklist = anytoks(hashproj, index[i], tokv);
      toks = tokenize(toklist,&ntoks);
      /*showtoks(toks,ntoks);*/
      run_search(toks);
      results[i] = result;
      if (result.count
	  && (best_findset < 0 
	      || (best_findset >= 0 && results[best_findset].count < result.count)))
	{
	  best_findset = i;
	  best_res_gran = res_gran;
	  vps[i] = vp;
	}
    }
  if (best_findset >= 0)
    {
#if 0
      fprintf(anyout, "%s %lu\n", index[best_findset], 
	      (unsigned long)results[best_findset].count);
#endif
      for (i = 0; index[i]; ++i)
	{
	  if (i == best_findset)
	    continue;
	  if (results[i].count)
	    fprintf(stderr, "se_any: %s %lu\n", index[i], (unsigned long)results[i].count);
	}
      vp = vps[best_findset];
      res_gran = best_res_gran;
      return_index = &index[best_findset][1];
      put_results(qp, &results[best_findset]);
      sdp->count = (int)results[best_findset].count;
      qx_count_file(sdp);
    }
  else
    {
      vp = vps[0];
      res_gran = best_res_gran;
      return_index = &index[0][1];
      put_results(qp, &results[0]);
      sdp->count = (int)results[0].count;
      qx_count_file(sdp);
    }

#if 0
  if (anyout_closable)
    fclose(anyout);
#endif
  
  return 0;
}

const char **
anytoks(const char *project, const char *index, const char **toks)
{
  const char **ret_toks = NULL;
  int ntoks = 0;
  char pbuf[strlen(project)+2];
  char ibuf[strlen(index)+2];
  if ('#' == *project)
    strcpy(pbuf, project);
  else
    sprintf(pbuf, "#%s", project);
  if ('!' == *index)
    strcpy(ibuf, index);
  else
    sprintf(ibuf, "!%s", index);
  while (toks[ntoks])
    ++ntoks;
  ret_toks = malloc((ntoks+3) * sizeof(const char *));
  ret_toks[0] = strdup(pbuf);
  ret_toks[1] = strdup(ibuf);
  for (ntoks = 0; toks[ntoks]; ++ntoks)
    ret_toks[ntoks+2] = toks[ntoks];
  ret_toks[ntoks+2] = NULL;
  return (const char **)ret_toks;
}
