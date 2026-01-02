#include <stdio.h>
#include <string.h>
#include "runexpat.h"

extern int verbose;

static const char *xmlns_url;

static void
sH(void *userData, const char *name, const char **atts)
{
  char *p = NULL;
  if ((p = strchr(name,'|')))
    {
      *p = '\0';
      xmlns_url = strdup(name);
      runexpat_stop();
    }
}

const char *
xmlns(const char *fn)
{
  const char *f[2];
  f[0] = fn;
  f[1] = NULL;
  runexpatNS(i_list,f,sH,NULL,"|");
  if (verbose)
    fprintf(stderr, "xmlns: found xmlns_url `%s'\n", xmlns_url);
  return xmlns_url;
}
