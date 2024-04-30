#include <oraccsys.h>
#include "isp.h"

const char *
isp_xmd_outline(Isp *ip)
{
  char pbuf[strlen(ip->project)+1];
  char oraccwww[strlen(oracc_builds())+strlen("/www/")];
  sprintf(oraccwww, "%s/www/", oracc_builds());
  strcpy(pbuf, ip->project);
  char *p = strrchr(pbuf,'/');
  if (!p)
    p = pbuf;
  char xsl[strlen(oraccwww)+strlen(pbuf)+strlen("/lib/scripts/xmdoutline.xsl0")];
  do
    {
      sprintf(xsl, "%s/%s/xmdoutline.xsl", oraccwww, pbuf);
      if (!access(xsl, R_OK))
	return (ccp)pool_copy((ucp)xsl, ip->p);
      p = strchr(pbuf,'/');
      if (p)
	*p = '\0';
      else
	p = pbuf;
    }
  while (p != pbuf);
  sprintf(xsl, "%s/lib/scripts/p3-xmd-div.xsl", oracc_builds());
  if (!access(xsl, R_OK))
    return (ccp)pool_copy((ucp)xsl, ip->p);
  else
    {
      ip->err = PX_ERROR_START "system XMD outline processor %s not found\n";
      ip->errx = (ccp)pool_copy((ucp)xsl, ip->p);
      return NULL;
    }
}
