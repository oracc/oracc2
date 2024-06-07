#include <dx.h>
#include <osh.h>

int
osh_wrapup(Job *jp)
{
  char cat[strlen(jp->log)+5];
  sprintf(cat, "cat %s", jp->log);
  fprintf(stdout, "osh[%d]: >>>>>>>%s>>>>>>>\n", jp->pid, jp->log);
  system(cat);
  fprintf(stdout, "osh[%d]: <<<<<<<%s<<<<<<<\n", jp->pid, jp->log);
  fprintf(stdout, "osh[%d]: %s completed in %i sec\n", jp->pid, jp->cmd, jp->time);
  osh_cleanup(jp);
  return 0;
}
