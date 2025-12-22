#include <oraccsys.h>
#include "wx.h"
#include "px.h"
#include "p4url.h"
#include "ccgi/ccgi.h"

/* Harsh and fast validation of CGI arguments; return 400 on any bad
 * char or any args that don't match the allowed format in qsoption.g
 */
const char *
p4url_validate(const char *value, const char *allow)
{
  const unsigned char *v = (const unsigned char *)value;
  while (*v)
    {
      if (*v < 128 && !isalnum(*v))
	{
	  static const char *ok = ",.%-_";
	  if (!strchr(ok, *v))
	    do400("WX reports: Illegal character in query string.");
	}
      else
	++v;
    }
  return value;
}
