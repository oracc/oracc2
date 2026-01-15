#include <oraccsys.h>

char *
findfile(const char *project, const char *file2find, const char *deflt)
{
  return findfile_dir(project, file2find, deflt, "www");
}

char *
findfile_dir(const char *project, const char *file2find, const char *deflt, const char *dir)
{
  int flen = strlen(file2find);
  if (strlen(deflt)>flen)
    flen = strlen(deflt);
  char pbuf[strlen(project)+1];
  strcpy(pbuf, project);
  char *p = strrchr(pbuf,'/');
  if (!p)
    p = pbuf;
  char fil[strlen(oracc_builds())+strlen("//0")+strlen(pbuf)+flen+strlen(dir)];
  do
    {
      sprintf(fil, "%s/%s/%s/%s", oracc_builds(), dir, pbuf, file2find);
      if (!access(fil, R_OK))
	{
	  fprintf(stderr, "findfile: using %s\n", fil);
	  return strdup(fil);
	}
      else
	fprintf(stderr, "findfile: %s not found\n", fil);
      p = strrchr(pbuf,'/');
      if (p)
	*p = '\0';
      else
	p = pbuf;
    }
  while (p != pbuf);
  if (deflt)
    {
      sprintf(fil, "%s/%s", oracc_builds(), deflt);
      if (!access(fil, R_OK))
	{
	  fprintf(stderr, "findfile: using %s\n", fil);
	  return strdup(fil);
	}
      else
	{
	  fprintf(stderr, "findfile: default file %s not found\n", fil);
	  return NULL;
	}
    }
  else
    return NULL;
}
