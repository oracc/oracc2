#include <oraccsys.h>

extern int atfd_flex_debug;
extern void atfdlex(void);

int
main(int argc, char **argv)
{
  atfd_flex_debug = 0;
  mesg_init();
  atfdlex();
  mesg_print(stderr);
}
