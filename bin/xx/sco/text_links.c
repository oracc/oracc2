#include <oraccsys.h>

/* load the .tlx file from PROJECT, PQX args into the HASH arg */
void
text_links(const char *project, const char *pqx, Hash *links, Pool *p)
{
  const char *tlxfile = expand(project, pqx, ".tlx");
  Roco *r = roco_load1(tlxfile);
  int i;
  for (i = 0; i < r->nlines; ++i)
    {
      char buf[strlen(project)+strlen(pqx)+strlen(r->rows[i][0])+3];
      sprintf(buf, "%s:%s~%s", project, pqx, r->rows[i][0]);
      hash_add(links, pool_copy((uccp)buf, p), r->rows[i][1]);
    }
}
