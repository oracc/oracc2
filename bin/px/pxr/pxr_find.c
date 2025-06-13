#include <oraccsys.h>
#include "../isp/isp.h"
#include "../pxdefs.h"
#include "pxr.h"

static const char *pxr_err;

char *
pxr_find_www_file(const char *project, const char *dir, const char *basename, const char *ext)
{
  static const char docroot[_MAX_PATH];
  sprintf(docroot, "%s", oracc());
  char *bufp = NULL;
  if (!project || !basename)
    return NULL;
  if (!dir)
    dir = "";
  if (!ext)
    ext = "";
  bufp = malloc(strlen(docroot) + strlen(project) + strlen(dir) + 
		strlen(basename) + strlen(ext) + strlen("////") + 1);
  sprintf(bufp,"%s/%s%s%s/%s%s%s",
	  docroot,project,
	  (*dir ? "/" : ""), (dir?dir:""),
	  basename,
	  (*ext ? "." : ""), (ext?ext:""));
  return bufp;
}

char *
pxr_find_gent(const char *project, const char *glos, const char *oid, const char *form)
{
  char *f = NULL;
  if (!strcmp(form, "html"))
    {
      char *cbd = pxr_find_www_file(project, "cbd", glos, NULL);
      char a[strlen(cbd)+strlen(oid)+strlen("/.html0")];
      sprintf(a, "%s/%s.html", cbd, oid);
      f = strdup(a);
      if (access(a, R_OK))
	{
	  pxr_err = px_err("unable to open %s: %s", f, strerror(errno));
	  return NULL;
	}
    }
  return f;
}

char *
pxr_find_glos(const char *project, const char *glos, const char *form)
{
  char *f = NULL;
  return f;
}

char *
pxr_find_pqx_file(const char *project, const char *pqid, const char *ext)
{
  char prefix[5], *bufp, pqx[8], *colon, *xproject = NULL;
  if (!project || !pqid || !ext)
    return NULL;

  if ((colon = strchr(pqid, ':')))
    {
      strcpy(pqx, colon+1);
      xproject = malloc(1+(colon-pqid));
      strncpy(xproject,pqid,colon-pqid);
      xproject[colon-pqid] = '\0';
    }
  else
    {
      xproject = (char*)project;
      strcpy(pqx, pqid);
    }
  

  if (!strcmp(ext, "tei"))
    {
      bufp = malloc((3 * strlen(pqx)) + strlen(xproject) + 6);
      sprintf(bufp,"%s/xml/%s/00tei/%s.xml",oracc(),xmlroot,xproject,pqx);
    }
  else
    {
      strncpy(prefix,pqx,4);
      prefix[4] = '\0';
      bufp = malloc(strlen(oracc_home()) + (4 * strlen(pqx)) + strlen(xproject) + 7);
      sprintf(bufp,"%s/bld/%s/%s/%s/%s.%s",oracc_home(),xproject,prefix,pqx,pqx,ext);
    }
  return bufp;
}

char *
pxr_find_pqx_xtr(const char *project, const char *pqid, const char *code, const char *lang)
{
  char prefix[5], *bufp, pqx[8], *colon, *xproject = NULL;
  if (!project || !pqid || !code || !lang)
    return NULL;

  if ((colon = strchr(pqid, ':')))
    {
      strcpy(pqx, colon+1);
      xproject = malloc(1+(colon-pqid));
      strncpy(xproject,pqid,colon-pqid);
      xproject[colon-pqid] = '\0';
    }
  else
    {
      xproject = (char*)project;
      strcpy(pqx, pqid);
    }  

  strncpy(prefix,pqid,4);
  prefix[4] = '\0';
  bufp = malloc((4 * strlen(pqx)) + strlen(xproject) + strlen(code) + strlen(lang) + 10);
  sprintf(bufp,"%s/bld/%s/%s/%s/%s_%s-%s.xtr",oracc_home(),xproject,prefix,pqx,pqx,code,lang);
  return bufp;
}

const char *
pxr_find_file(Isp *ip, const char *file2find, const char *default)
{
  int flen = strlen(file2find);
  if (strlen(default)>flen)
    flen = strlen(default);
  char pbuf[strlen(ip->project)+1];
  strcpy(pbuf, ip->project);
  char *p = strrchr(pbuf,'/');
  if (!p)
    p = pbuf;
  char fil[strlen(oracc_builds())+strlen("/www/0")+strlen(pbuf)+flen];
  do
    {
      sprintf(xsl, "%s/www/%s/%s", oracc_builds(), pbuf, file2find);
      if (!access(xsl, R_OK))
	return (ccp)pool_copy((ucp)fil, ip->p);
      else
	fprintf(stderr, "isp_xmd_outline: %s not found\n", xsl);
      p = strrchr(pbuf,'/');
      if (p)
	*p = '\0';
      else
	p = pbuf;
    }
  while (p != pbuf);
  sprintf(xsl, "%s/lib/scripts/%s", oracc_builds(), default);
  if (!access(xsl, R_OK))
    return (ccp)pool_copy((ucp)fil, ip->p);
  else
    {
      ip->err = PX_ERROR_START "system default file %s not found\n";
      ip->errx = (ccp)pool_copy((ucp)fil, ip->p);
      return NULL;
    }
}
