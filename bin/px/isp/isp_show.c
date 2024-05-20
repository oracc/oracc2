#include <oraccsys.h>
#include "../px.h"
#include "isp.h"

void
isp_argv(FILE *fp, Isp *ip)
{
  int i;
  for (i = 0; i < ip->argc; ++i)
    {
      if (i)
	fputc(' ', fp);
      fputs(ip->argv[i], fp);
    }
}

int
cmpstringp(const void *p1, const void *p2)
{
  return strcmp(* (char * const *) p1, * (char * const *) p2);
}

static void
isp_show_cookies(FILE *fp, char *s)
{
  char *e = strchr(s, '=');
  while (s < e)
    fputc(*s++, fp);
  fputc('=', fp);
  ++s;
  while ((e = strstr(s, "; ")))
    {
      fputs("\n\t", fp);
      while (s < e)
	fputc(*s++, fp);
      fputc(';', fp);
      s += 2;
    }
  fputc('\n', fp);
}

void
isp_environment(FILE *fp, Isp *ip)
{
  extern char **environ;
  char **s = environ, **e;

  int i = 0;
  for (e = s; *e; ++e)
    ++i;

  qsort(s, i, sizeof(char **), cmpstringp);
  
  for (; *s; s++)
    {
      if (1 || !strncmp(*s, "HTTP", 4))
	{
	  if (!strncmp(*s, "HTTP_COOKIE", strlen("HTTP_COOKIE")))
	    isp_show_cookies(fp, *s);
	  else
	    fprintf(fp, "%s\n", *s);
	}
    }
}

void
isp_show(FILE *fp, Isp *ip)
{
#if 1
  px_print_ip(fp,ip);
#else
  fprintf(fp, "isp status:\n");
  fprintf(fp, "\tweb=%d; oracc=%s; project=%s\n", ip->web, ip->oracc, ip->project);
  fprintf(fp, "\tlist: name=%s; type=%s; lang=%s; method=%s;\n"
	  "\t\tpath=%s;\n"
	  "\t\tdbpath=%s; dbname=%s\n",
	  ip->list_name, ip->lloc.type, ip->lloc.lang, ip->lloc.method,
	  ip->lloc.path,
	  ip->lloc.dbpath, ip->lloc.dbname);
  fprintf(fp, "\tdors=%s; zoom=%s; page=%s; cemd=%s; xhmd=%s\n",
	  ip->dors, ip->zoom, ip->page, ip->cemd, ip->xhmd);
  fprintf(fp, "\thost=%s; host_path=%s\n", ip->host, ip->host_path);

  fprintf(fp, "<h2>Cache and Files</h2>\n");
  fprintf(fp, "\tcache=%s\n\tzoomdiv=%s\n\tmetadiv=%s\n\tcontent=%s\n",
	  ip->cache.sub, ip->cache.zout, ip->cache.meta, ip->cache.page);
#endif
}
