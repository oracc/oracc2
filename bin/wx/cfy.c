#include <oraccsys.h>
#include "wx.h"
#include "ccgi/ccgi.h"

void
cfy(void)
{
  CGI_varlist *v = CGI_get_post(NULL, NULL);

  printf("Hello from cfy\n");
  show_vl(v);
  
  exit(0);
}

static void
cfy_call(char **vec)
{
  execv("/bin/sh", vec);
}

static char const**
cfy_vec(CGI_varlist *vl)
{
  const char *name;
  char **vec;
  int i;

  for (i = 0, name = CGI_first_name(vl); name != 0; name = CGI_next_name(vl))
    ++i;

  --i; /* skip "data" */
  i *= 2;
  
  vec = malloc((i+3) * sizeof(const char*));

  vec[0] = "/bin/sh";
  for (i = 1, name = CGI_first_name(vl);
       name != 0 && strcmp(name, "data");
       name = CGI_next_name(vl))
    {
      vec[i++] = "name";
      vec[i++] = CGI_lookup(vl,name);
    }
  vec[i] = NULL;

  return vec;
}

static void
cfy_data(CGI_varlist *v)
{
  const char *data = CGI_lookup_all(v, "data");
  if (data)
    {
      char pid[8];
      sprintf(pid, "%d", getpid());
      char buf[strlen("/tmp/.cfy0")+strlen(pid)];
      sprintf(buf, "/tmp/%s.cfy", pid);
      FILE *d = fopen(buf, "w");
      if (d)
	{
	  fwrite(data, 1, strlen(data), d);
	  fclose(d);
	  v = CGI_add_var(v, "datafile", strdup(buf));
	}      
    }  
}
