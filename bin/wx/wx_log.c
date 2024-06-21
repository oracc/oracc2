#include <oraccsys.h>
#include <time.h>

void
wx_log(const char *u, const char *q, const char *r)
{
  #define TM_MAX 256
  char tmbuf[TM_MAX];
  time_t t = time(NULL);
  struct tm *gmt = gmtime(&t);
  strftime(tmbuf, TM_MAX, "<date>%Y-%m-%dT%H:%M</date>", gmt);

  const char *log = "/var/log/oracc-wx.log";
  
  FILE *fp = fopen(log, "r+");
  fprintf(fp, "%s\t%s\t%s\t%s\n", tmbuf, u, q, r);
  fclose(fp);
}
