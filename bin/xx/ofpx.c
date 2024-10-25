#include <oraccsys.h>
#include "ofp.h"

int
main(int argc, char **argv)
{
  Ofp *ofp = ofp_load("-");
#if 1
  ofp_xml(ofp, stdout);
#else
  ofp_dump(ofp, stdout);
#endif
}
