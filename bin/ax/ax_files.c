#include <oraccsys.h>
#include "ax.h"

static char *
atffile_of(const char *d)
{
  const char *fn = expand(NULL, d, "atf");
  if (fn)
    return strdup(fn);
  else
    {
      fprintf(stderr, "ax: invalid ATF path `%s': must end with a valid PQX-id\n", d);
      return NULL;
    }
}

static char *
trafile_of(const char *d)
{
  const char *fn = expand_xtr(NULL, d, "project", "en");
  return strdup(fn);
}

static char *
xtffile_of(const char *d)
{
  const char *fn = expand(NULL, d, "xtf");
  return strdup(fn);
}

static char *
tlxfile_of(const char *d)
{
  const char *fn = expand(NULL, d, "tlx");
  return strdup(fn);
}

void
ax_files(const char *prj, const char *pqx)
{
  char qidbuf[prj ? strlen(prj)+strlen(pqx)+2 : 0];
  const char *qid = qidbuf;
  if (prj)
    sprintf(qidbuf, "%s:%s", prj, pqx);
  else
    qid = (char*)pqx;

  atf_files.atf = atffile_of(qid);
  atf_files.tra = trafile_of(qid);
  atf_files.xtf = xtffile_of(qid);
  atf_files.tlx = tlxfile_of(qid);
}
