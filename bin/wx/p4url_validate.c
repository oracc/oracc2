#include <oraccsys.h>
#include "wx.h"
#include "px.h"
#include "p4url.h"
#include "ccgi/ccgi.h"

/* Harsh and fast validation of CGI arguments of args that don't match
   the allowed format in qsoption.g
 */
const char *
p4url_validate(const char *name, const char *value, const char *allow)
{
  const unsigned char *v = (const unsigned char *)value;
  p4url_guard(name, v);
  return value;
}

/* emit HTTP 400 on any bad char; return input string if no bad chars */
const unsigned char *
p4url_guard(const char *name, const unsigned char *v)
{
  if (!v)
    return NULL;

  /* Allow signatures to pass through because they can legitimately
     contain almost anything; they are handled carefully in
     px/sigmap.c to ensure they are not returned to the browser */
  if (!strcmp(name, "sig"))
    return v;

  const unsigned char *vv = v;
  while (*v)
    {
      if (*v < 128 && !isalnum(*v))
	{
	  static const char *ok = ",.%-_ \t()|&@";
	  if (!strchr(ok, *v))
	    {
	      if (name)
		fprintf(stderr, "p4url_guard: rejecting `%s=%s' because of char `%c'\n",
			name,vv,*v);
	      else
		fprintf(stderr, "p4url_guard: rejecting `%s' because of char `%c'\n",
			vv,*v);
	      do400("WX reports: Illegal character in query string.");
	    }
	}
      ++v;
    }
  return vv;
}
