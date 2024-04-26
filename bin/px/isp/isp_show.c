#include <oraccsys.h>
#include "isp.h"

void
isp_show(FILE *fp, Isp *ip)
{
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
}
