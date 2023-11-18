#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

const char *varoracc = "/home/oracc/tmp/sop";

char *
dx_session(char **sesh_id)
{
  char *template;
  char *tmpdir = NULL;

  template = malloc(strlen(varoracc) + 10);
  sprintf(template, "%s/dxXXXXXX", varoracc);
  fprintf(stderr, "sesh: sesh_template=%s\n", template);
  tmpdir = (char*)mkdtemp(template);  

  if (tmpdir)
    {
      tmpdir = strdup(tmpdir);
      chmod(tmpdir, 02750);
      char *basename = tmpdir+strlen(tmpdir);
      while (basename > tmpdir && '/' != basename[-1])
	--basename;
      *sesh_id = basename;
      fprintf(stderr, "sesh: tmpdir=%s; sesh_id=%s\n", tmpdir, basename);
      return tmpdir;
    }
  else
    {
      fprintf(stderr, "dx: failed to create tmpdir from template %s\n", template);
      return NULL;
    }
}
