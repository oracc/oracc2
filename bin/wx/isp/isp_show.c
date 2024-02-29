#include <oraccsys.h>
#include "isp.h"

void
isp_show(FILE *fp, struct isp *ip)
{
  fprintf(fp, "ispx status:\n");
  fprintf(fp, "\tweb=%d; oracc=%s; project=%s; list=%s\n", ip->web, ip->oracc, ip->project, ip->list_name);
  fprintf(fp, "\tzoom=%s; page=%s; cemd=%s; xhmd=%s\n", ip->zoom, ip->page, ip->cemd, ip->xhmd);
  fprintf(fp, "\tlist_path=%s; host=%s; host_path=%s\n", ip->list_path, ip->host, ip->host_path);
}
