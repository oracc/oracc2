#include <oraccsys.h>

const char *project;
extern int atfd_flex_debug;
extern void atfdlex(void);

int
main(int argc, char **argv)
{
  project = argv[1];
  if (!project)
    {
      fprintf(stderr, "%s: must give project on command line. Stop.\n", argv[0]);
      exit(1);
    }
  
  atfd_flex_debug = 0;
  mesg_init();
  atfdlex();
  mesg_print(stderr);
}
