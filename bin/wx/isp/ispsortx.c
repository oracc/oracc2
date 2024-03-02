#include <oraccsys.h>
#include "iss.h"

extern int ispsort(const char *arg_project, const char *arg_listfile, const char *arg_sortkeys);

const char *arg_listfile, *arg_project, *arg_sort_keys;

int
main(int argc, char **argv)
{
  options(argc,argv,"l:p:s:");
  ispsort(arg_project, arg_listfile, arg_sort_keys);
  exit(0);
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'l':
      arg_listfile = arg;
      break;
#if 0      
    case 'm':
      arg_use_linkmap = 1;
      break;
#endif
    case 'p':
      arg_project = arg;
      break;
    case 's':
      arg_sort_keys = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "ispsortx";
int major_version = 1, minor_version = 1, verbose = 0;
const char *usage_string = "[file]";
void
help ()
{

}
