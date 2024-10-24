#include <oraccsys.h>
#include "ofp.h"

int
main(int argc, char **argv)
{
  Ofp *ofp = ofp_load("-");
  ofp_xml(ofp, stdout);
}
