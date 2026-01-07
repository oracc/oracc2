#include <oraccsys.h>

const char *
findfile(const char *project, const char *file2find, const char *deflt)
{
  int flen = strlen(file2find);
  if (strlen(deflt)>flen)
    flen = strlen(deflt);
  char pbuf[strlen(project)+1];
  strcpy(pbuf, project);
  char *p = strrchr(pbuf,'/');
  if (!p)
    p = pbuf;
  char fil[strlen(oracc_builds())+strlen("/www/0")+strlen(pbuf)+flen];
  do
    {
      sprintf(fil, "%s/www/%s/%s", oracc_builds(), pbuf, file2find);
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
  sprintf(fil, "%s/%s", oracc_builds(), deflt);
  if (!access(fil, R_OK))
    {
      fprintf(stderr, "findfile: using %s\n", fil);
      return strdup(fil);
    }
  else
    {
      fprintf(stderr, "px_find_file: default file %s not found\n", fil);
      return NULL;
    }
}
