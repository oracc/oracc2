#include <stdlib.h>
#include <stdio.h>

extern void cdli2xtfset_debug(int);
extern int cdli2xtfflextrace, cdli2xtf_flex_debug;
extern int cdli2xtflex(void);

const char *xmlns="xmlns:x=\"http://oracc.org/ns/xtf/1.0\" xmlns:g=\"http://oracc.org/ns/gdl/1.0\"";

int
main(int argc, char **argv)
{
  cdli2xtfflextrace = 0;
  cdli2xtfset_debug(0);
  printf("<x:xtf project=\"cdli\" %s>", xmlns);
  while (cdli2xtflex())
    ;
  printf("</x:xtf>");
}
