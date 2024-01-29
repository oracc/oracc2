#include <oraccsys.h>

/* OSE : Oracc Search Engine */

#include "ose.h"

void
ose_ce_cfg(const char *project, const char *index,
       const char *ce_tag, const char *ce_ext, 
       enum oce_files ce_type, const char **proxies)
{
  const char *cfg_fn = ose_file(project,index,"ce.cfg");
  FILE *cfg_fp = fopen(cfg_fn,"w");
  if (!cfg_fp)
    {
      perror(cfg_fn);
      exit(1);/*FIXME*/
    }
  fprintf(cfg_fp,"%d",ce_type);
  if (ce_ext)
    fprintf(cfg_fp," %s",ce_ext);
  fprintf(cfg_fp," %s%c",ce_tag,0);
  if (proxies)
    while (*proxies)
      fprintf(cfg_fp,"%s%c",*proxies++,0);
  fprintf(cfg_fp,"%c",0);
  fclose(cfg_fp);
}

const char *
ose_dir(const char *project, const char *xname)
{
  static char _dirbuf[_MAX_PATH];
  sprintf(_dirbuf,"%s/pub/%s/%s",oracc_var(),project,xname);
  return _dirbuf;
}

const char *
ose_file(const char *project, const char *xname, const char *fname)
{
  char *fbuf = (char*)ose_dir(project,xname);
  strcat(fbuf,"/");
  return strcat(fbuf,fname);
}

void
ose_wid2loc8(const char *id, const char *lang, struct location8 *l8p)
{
  if ('v' == *id)
    {
      l8p->text_id = atoi(id+1);
      id += 8;
      if (*id)
	{
	  l8p->unit_id = atoi(id);
	  while (*id && '.' != *id)
	    ++id;
	  if (*id)
	    {
	      ++id;
	      l8p->word_id = atoi(id);
	    }
	}
    }
  else
    fprintf(stderr,"wid2loc8: %s: malformed ID\n", id);
}
