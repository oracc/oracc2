#include <oraccsys.h>
#include <xpd.h>
#include "../px.h"

void
pui_at_ceid(Isp *ip, FILE *fp)
{
  fputs(ip->ceid, fp);
}

void
pui_at_translations(Isp *ip, FILE *fp)
{
}

void
pui_at_current_page(Isp *ip, FILE *fp)
{
  fputs(ip->page, fp);
}

void
pui_at_active_pages(Isp *ip, FILE *fp)
{
  int psize = atoi(ip->psiz);
  int npages = ip->md1.zimx / psize;
  if (ip->md1.zimx % psize)
    ++npages;
  fputs(itoa(npages), fp);  
}

void
pui_at_active_items(Isp *ip, FILE *fp)
{
  fputs(itoa(ip->md1.zimx), fp);
}

void
pui_at_srch_results(Isp *ip, FILE *fp)
{
  fputs("(no search results)", fp);
}

void
pui_at_environment(Isp *ip, FILE *fp)
{
  isp_environment(fp, ip);  
}

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
  fprintf(fp, "%s &#x2014; %s", xpd_option(ip->xpd, "logo"), xpd_option(ip->xpd, "name"));
}

void
pui_at_project(Isp *ip, FILE *fp)
{
  fputs(ip->project,fp);
}

void
pui_at_menu(Isp *ip, FILE *fp)
{
  file_copy(ip->cache.zout, "-");
}

void
pui_at_content(Isp *ip, FILE *fp)
{
  if (ip->item)
    (void)pui_output(ip, stdout, pui_filetext("p4item.xml"));
  else
    file_copy(ip->cache.page, "-");
}
