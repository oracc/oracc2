#include <oraccsys.h>
#include "ofp.h"

int tsv_dump = 0;
int xml_dump = 0;

int
main(int argc, char **argv)
{
  options(argc, argv, "tx");

  if (!tsv_dump && !xml_dump)
    tsv_dump = 1;
  
  Ofp *ofp = ofp_load("-");

  if (xml_dump)
    ofp_xml(ofp, stdout);

  if (tsv_dump)
    ofp_dump(ofp, stdout);
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 't':
      tsv_dump = 1;
      break;
    case 'x':
      xml_dump = 1;
      break;
    }
  return 0;
}

void help(void) { }
