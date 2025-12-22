#include <oraccsys.h>
#include "wx.h"
#include "px.h"
#include "p4url.h"
#include "ccgi/ccgi.h"

/* Harsh and fast validation of CGI arguments of args that don't match
   the allowed format in qsoption.g
 */
const char *
p4url_validate(const char *value, const char *allow)
{
  const unsigned char *v = (const unsigned char *)value;
  p4url_guard(v);
  return value;
}

/* emit HTTP 400 on any bad char; return input string if no bad chars */
const unsigned char *
p4url_guard(const unsigned char *v)
{
  if (!v)
    return NULL;
  const unsigned char *vv = v;
  while (*v)
    {
      if (*v < 128 && !isalnum(*v))
	{
	  static const char *ok = ",.%-_";
	  if (!strchr(ok, *v))
	    do400("WX reports: Illegal character in query string.");
	}
      ++v;
    }
  return vv;
}
