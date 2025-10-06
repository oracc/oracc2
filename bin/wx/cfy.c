#include <oraccsys.h>
#include "wx.h"
#include "ccgi/ccgi.h"

static void cfy_call(char* const *vec);
static char * const*cfy_vec(CGI_varlist *vl);
static void cfy_data(CGI_varlist *v);

void
cfy(void)
{
  CGI_varlist *v = CGI_get_post(NULL, NULL);
  cfy_data(v);
  dump_vl(v);
  cfy_call(cfy_vec(v));
  fprintf(stderr, "execv /bin/sh cfy.sh failed\n");  
  exit(1);
}

static void
cfy_call(char * const*vec)
{
  dump_vec("/bin/sh", vec);
  execv("/bin/sh", vec);
}

static char *const*
cfy_vec(CGI_varlist *vl)
{
  const char *name;
  char **vec;
  int i;

  for (i = 0, name = CGI_first_name(vl); name != 0; name = CGI_next_name(vl))
    ++i;

  i *= 2;
  
  vec = malloc((i+3) * sizeof(const char*));

  vec[i=0] = "/bin/sh";
  vec[i=1] = "/Users/stinney/orc/bin/cfy.sh";
  for (i=2, name = CGI_first_name(vl); name != 0; name = CGI_next_name(vl))
    {
      if (strcmp(name, "data"))
	{
	  vec[i++] = (char*)name;
	  vec[i++] = (char*)CGI_lookup(vl,name);
	}
    }
  vec[i] = NULL;

  return (char * const *)vec;
}

static void
cfy_data(CGI_varlist *v)
{
  CGI_value *dataval = CGI_lookup_all(v, "data");
  if (dataval)
    {
      char *data = dataval[0];
      char pid[8];
      sprintf(pid, "%d", getpid());
      char buf[strlen("/tmp/.cfy0")+strlen(pid)];
      sprintf(buf, "/tmp/%s.cfy", pid);
      fprintf(stderr, "cfy_data: writing %s to %s\n", data, buf);
      FILE *d = fopen(buf, "w");
      if (d)
	{
	  fwrite((char*)data, 1, strlen((char*)data), d);
	  fclose(d);
	  v = CGI_add_var(v, "file", strdup(buf));
	}      
    }  
}
