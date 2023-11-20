#include <../dx.h>
#include <osh.h>

int
osh_wrapup(Job *jp)
{
  char cat[strlen(jp->log)+5];
  sprintf(cat, "cat %s", jp->log);
  fprintf(stdout, "osh[%d]: session log begins\n", jp->pid);
  system(cat);
  fprintf(stdout, "osh[%d]: session log ends\n", jp->pid);
  fprintf(stdout, "osh[%d]: session %s completed in %i sec\n", jp->pid, jp->sesh, jp->time);
  osh_cleanup(jp);
  return 0;
}
