#include <oraccsys.h>
#include <stdarg.h>
#include "px.h"

const char *
px_mesg(const char *wpq, const char *fmt, ...)
{
  const char *ret = NULL;
  if (fmt)
    {
      va_list ap;
      va_start(ap, fmt);
      mesg_averr(NULL, fmt, ap);
      va_end(ap);
      const char *e = mesg_pop();
      char m[strlen(wpq)+strlen(e)+1];
      sprintf(m, "%s%s", wpq, e);
      /*free((char*)e);*//*NO: this is pool mem*/
      ret = strdup(m);
    }
  return ret;
}
