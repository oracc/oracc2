#include <oraccsys.h>
#include "px.h"

const char *
px_find_file(Isp *ip, const char *file2find, const char *deflt)
{
  int flen = strlen(file2find);
  if (strlen(deflt)>flen)
    flen = strlen(deflt);
  char pbuf[strlen(ip->project)+1];
  strcpy(pbuf, ip->project);
  char *p = strrchr(pbuf,'/');
  if (!p)
    p = pbuf;
  char fil[strlen(oracc_builds())+strlen("/www/0")+strlen(pbuf)+flen];
  do
    {
      sprintf(fil, "%s/www/%s/%s", oracc_builds(), pbuf, file2find);
      if (!access(fil, R_OK))
	return (ccp)pool_copy((ucp)fil, ip->p);
      else
	fprintf(stderr, "isp_xmd_outline: %s not found\n", fil);
      p = strrchr(pbuf,'/');
      if (p)
	*p = '\0';
      else
	p = pbuf;
    }
  while (p != pbuf);
  sprintf(fil, "%s/lib/scripts/%s", oracc_builds(), deflt);
  if (!access(fil, R_OK))
    return (ccp)pool_copy((ucp)fil, ip->p);
  else
    {
      ip->err = PX_ERROR_START "system default file %s not found\n";
      ip->errx = (ccp)pool_copy((ucp)fil, ip->p);
      return NULL;
    }
}
