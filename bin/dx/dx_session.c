#include <dx.h>

const char *varoracc = "/home/oracc/tmp/ipc";

char *
dx_session(char **sesh_id)
{
  char *template;
  char *tmpdir = NULL;

  template = malloc(strlen(varoracc) + 10);
  sprintf(template, "%s/dxXXXXXX", varoracc);
  fprintf(stderr, "%s: sesh_template=%s\n", progname, template);
  tmpdir = (char*)mkdtemp(template);  

  if (tmpdir)
    {
      tmpdir = strdup(tmpdir);
      chmod(tmpdir, 02750);
      char *basename = tmpdir+strlen(tmpdir);
      while (basename > tmpdir && '/' != basename[-1])
	--basename;
      *sesh_id = basename;
      fprintf(stderr, "%s: tmpdir=%s; sesh_id=%s\n", progname, tmpdir, basename);
      return tmpdir;
    }
  else
    {
      fprintf(stderr, "dx: failed to create tmpdir from template %s\n", template);
      return NULL;
    }
}
