#include "../px.h"

void
pui_at_error(Isp *ip, FILE *fp)
{
  fprintf(fp, ip->errx ? ip->err : PX_ERROR_START "%s", ip->errx ? ip->errx : ip->err);
}

void
pui_at_status(Isp *ip, FILE *fp)
{
  isp_show(fp, ip);
}
