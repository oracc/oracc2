#include <oraccsys.h>
#include "isp.h"

#if 0
/* Find a file in the given dir anywhere in the project's path,
 * starting from the deepest subproject and working up to parent
 */

const char *
find_in_project(const char *project, const char *dir, const char *file)
{
  if (!project || !dir || !file)
    return NULL;
  char p[strlen(project)+1];
  char f[strlen(project)+strlen(dir)+strlen(file)+3];
  strcpy(p, project);
  int last_try = 0;
  while (*p)
    {
      strcpy(f, "%s/%s/%s", p, dir, file);
      if (!access(f, R_OK))
	return f;
      else if (last_try)
	return NULL;
      char *slash = strrchr(p, '/');
      if (slash)
	*slash = '\0';
      else
	last_try = 1;
    }
  return NULL;
}
#endif

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
      else
	fprintf(stderr, "isp_xmd_outline: %s not found\n", xsl);
      p = strrchr(pbuf,'/');
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
