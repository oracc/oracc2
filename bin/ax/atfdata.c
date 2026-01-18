#include <oraccsys.h>

const char *cat_mode;
const char *catproj;
const char *project;
extern int x_mode; /* extract ID and NAME for X-IDs */
extern int atfd_flex_debug;
extern void atfdlex(void);

int
main(int argc, char **argv)
{
  options(argc, argv, "c:j:p:xX");
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
    case 'X':
      x_mode = 2;
    case 'x':
      printf("id_text\tdesignation\tperiod\tprovenience\n");
      if (x_mode)
	exit(0);
      x_mode = 1;
      break;
    }
  return 0;
}
