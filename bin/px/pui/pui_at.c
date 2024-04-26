#include <oraccsys.h>
#include <xpd.h>
#include "../px.h"

void
pui_at_error(Isp *ip, FILE *fp)
{
  fprintf(fp, ip->errx ? ip->err : PX_ERROR_START "%s", ip->errx ? ip->errx : ip->err);
}

void
pui_at_invocation(Isp *ip, FILE *fp)
{
  isp_argv(fp, ip);
}

void
pui_at_status(Isp *ip, FILE *fp)
{
  isp_show(fp, ip);
}

void
pui_at_state(Isp *ip, FILE *fp)
{
  fputs("page", fp);
}

void
pui_at_title(Isp *ip, FILE *fp)
{
  fprintf(fp, "%s : %s", xpd_option(ip->xpd, "logo"), xpd_option(ip->xpd, "name"));
}

void
pui_at_menu(Isp *ip, FILE *fp)
{
  fputs("MENU",fp);
}

void
pui_at_content(Isp *ip, FILE *fp)
{
  fputs("CONTENT",fp);
}
