#include <oraccsys.h>
#include "wpx_lib.h"

static const char *wpx_err;

void
wpx_cat_html(const char *form, const char *file)
{
  wpx_print_hdr();
  execl("/bin/cat", "cat", file, NULL);
  wpx_err = strerror(errno);
}

void
wpx_cat_xml(const char *form, const char *file)
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
