#include <oraccsys.h>
#include <time.h>

void
wx_log(const char *u, const char *q, const char *r)
{
  #define TM_MAX 256
  char tmbuf[TM_MAX];
  time_t t = time(NULL);
  struct tm *gmt = gmtime(&t);
#if 1
  strftime(tmbuf, TM_MAX, "%Y-%m-%dT%H:%M", gmt);
#else
  strftime(tmbuf, TM_MAX, "<date>%Y-%m-%dT%H:%M</date>", gmt);
#endif
  
  const char *log = "/var/log/oracc-wx.log";
  if (!r)
    r = "";
  
  FILE *fp = NULL;
  if ((fp = fopen(log, "r+")))
    {
      fprintf(fp, "%s\t%s\t%s\t%s\n", tmbuf, u, q, r);
      fclose(fp);
    }
  else
    fprintf(stderr, "wx_log: %s\t%s\t%s\t%s\n", tmbuf, u, q, r);    
}
