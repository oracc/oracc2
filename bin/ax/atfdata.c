#include <oraccsys.h>

const char *cat_mode;
const char *catproj;
const char *project;
extern int atfd_flex_debug;
extern void atfdlex(void);

int
main(int argc, char **argv)
{
  options(argc, argv, "c:j:p:");
  if (!project)
    {
      fprintf(stderr, "%s: must give project on command line. Stop.\n", argv[0]);
      exit(1);
    }
  if (cat_mode && (!strcmp(cat_mode, "local") || !strcmp(cat_mode, "custom")))
    catproj = project;
  else
    catproj = "cdli";
      
  atfd_flex_debug = 0;
  mesg_init();
  atfdlex();
  mesg_print(stderr);
}

void help(void) { fprintf(stderr, "atfdatax: -c [cat_mode] -p [project]\n"); }
int
opts(int c, const char *arg)
{
  switch (c)
    {
    case 'c':
      cat_mode = arg;
      break;
    case 'j':
    case 'p':
      project = arg;
      break;
    }
  return 0;
}
