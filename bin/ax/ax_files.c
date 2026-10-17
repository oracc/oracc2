#include <oraccsys.h>
#include "ax.h"

static const char *static_atffile = "ax.atf";
static const char *static_tlxfile = "ax.tlx";
static const char *static_trafile = "ax.xtr";
static const char *static_xmlfile = "ax.xml";

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
  if (!prj)
    return;
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

void
ax_files_static(void)
{
  atf_files.atf = static_atffile;
  atf_files.tlx = static_tlxfile;
  atf_files.tra = static_trafile;
  atf_files.xtf = static_xmlfile;
}
