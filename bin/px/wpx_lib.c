#include <oraccsys.h>
#include "wpx_lib.h"

static const char *wpx_err;

void
wpx_cat_file(const char *form, const char *file)
{
  wpx_print_hdr_xml();
  execl("/bin/cat", "cat", file, NULL);
  wpx_err = strerror(errno);
}

const char *
wpx_last_error(void)
{
  return wpx_err;
}

const char *
cgi_arg(const char *arg, const char *val)
{
  char *c = malloc(strlen(arg)+strlen(val)+2);
  sprintf(c, "%s=%s", arg, val);
  return c;
}

void
do302(const char *loc)
{
  printf("Status: 302 Found\nLocation: %s\n\n", loc);
  exit(0);
}

void
do400(const char *msg400)
{
  puts("HTTP/1.1 400 Bad Request\n");
  puts(msg400);
  exit(0);
}

void
do404(void)
{
  wpx_print_hdr();
  /*execl("/bin/cat", "cat", "@@ORACC@@/www/404.html", NULL);*/
  printf("404\n");
}

char *
wpx_find_www_file(const char *project, const char *dir, const char *basename, const char *ext)
{
  static const char *docroot = "/home/oracc/www"; /* need to @@ this */
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
wpx_find_art_file(const char *project, const char *glos, const char *oid, const char *form)
{
  char *f = NULL;
  if (!strcmp(form, "html"))
    {
      char *cbd = wpx_find_www_file(project, "cbd", glos, NULL);
      char a[strlen(cbd)+strlen(oid)+strlen("/.html0")];
      sprintf(a, "%s/%s.html", cbd, oid);
      f = strdup(a);
      if (access(a, R_OK))
	{
	  wpx_err = px_err("unable to open %s: %s", f, strerror(errno));
	  return NULL;
	}
    }
  return f;
}

char *
wpx_find_glo_file(const char *project, const char *glos, const char *form)
{
  char *f = NULL;
  return f;
}

char *
wpx_find_pqx_file(const char *project, const char *pqid, const char *ext)
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
      const char *xmlroot = "/home/oracc/xml"; /* need to @@ this */
      bufp = malloc((3 * strlen(pqx)) + strlen(xproject) + 6);
      sprintf(bufp,"%s/%s/00tei/%s.xml",xmlroot,xproject,pqx);
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
wpx_find_pqx_xtr(const char *project, const char *pqid, const char *code, const char *lang)
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

void
wpx_print_hdr(void)
{
  fputs("Content-type: text/html; charset=utf-8\nAccess-Control-Allow-Origin: *\n\n", stdout);
  fflush(stdout);
}

void
wpx_print_hdr_text(void)
{
  fputs("Content-type: text/plain; charset=utf-8\n\n", stdout);
  fflush(stdout);
}

void
wpx_print_hdr_xml(void)
{
  fputs("Content-type: text/xml; charset=utf-8\nAccess-Control-Allow-Origin: *\n\n", stdout);
  fflush(stdout);
}

void
wpx_print_hdr_json()
{
  fputs("Content-type: application/json\n\n", stdout);
  fflush(stdout);
}

void
wpx_print_hdr_pdf(const char *pname)
{
  fprintf(stdout,
	  "Content-type: application/pdf\nContent-disposition: attachment; filename=\"%s\"\n\n", pname);
  fflush(stdout);
}

void
wpx_print_hdr_zip(const char *zname)
{
  fprintf(stdout,
	  "Content-type: application/zip\nContent-disposition: attachment; filename=\"%s\"\n\n", zname);
  fflush(stdout);
}
