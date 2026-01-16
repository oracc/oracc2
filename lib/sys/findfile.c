#include <oraccsys.h>

int ff_verbose;

char *
findfile(const char *project, const char *file2find, const char *deflt)
{
  return findfile_dir(project, file2find, deflt, "www");
}

char *
findfile_dir(const char *project, const char *file2find, const char *deflt, const char *dir)
{
  int flen = strlen(file2find);
  if (deflt && strlen(deflt)>flen)
    flen = strlen(deflt);
  char pbuf[strlen(project)+1];
  strcpy(pbuf, project);
  char *p = strrchr(pbuf,'/');
  if (!p)
    p = pbuf;
  const char *dir00;
  if (!strcmp(dir, "lib"))
    dir00 = "00lib";
  else
    dir00 = dir;
  char fil[strlen(oracc_builds())+strlen("//0")+strlen(pbuf)+flen+strlen(dir00)];
  do
    {
      if ('0' == *dir00)
	sprintf(fil, "%s/%s/%s/%s", oracc_builds(), pbuf, dir00, file2find);
      else
	sprintf(fil, "%s/%s/%s/%s", oracc_builds(), dir00, pbuf, file2find);
      if (!access(fil, R_OK))
	{
	  if (ff_verbose)
	    fprintf(stderr, "findfile: using %s\n", fil);
	  return strdup(fil);
	}
      else if (ff_verbose)
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
	  if (ff_verbose)
	    fprintf(stderr, "findfile: using %s\n", fil);
	  return strdup(fil);
	}
      else
	{
	  if (ff_verbose)
	    fprintf(stderr, "findfile: default file %s not found\n", fil);
	  return NULL;
	}
    }
  else
    return NULL;
}
