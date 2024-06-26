#include <dx.h>
#include <osh.h>

#define osh_maybe_free(p) if (jp->p) free(jp->p)
void
osh_cleanup(Job *jp)
{
  if (jp)
    {
      osh_maybe_free(cmdv);
      osh_maybe_free(cmd);
      osh_maybe_free(user);
      osh_maybe_free(project);
      osh_maybe_free(tmp);
      osh_maybe_free(log);
      osh_maybe_free(sesh);
      osh_maybe_free(statusfile);
      osh_maybe_free(phome);
      osh_maybe_free(pwork);
      memset(jp, '\0', sizeof(Job));
    }
}
