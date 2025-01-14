#include <oraccsys.h>
#include "../../lib/ofp/ofp.h"

int xml_dump = 1;

const char *list_arg = NULL;
const char *def_file = NULL;
const char *xml_file = "-";

int
main(int argc, char **argv)
{
  options(argc, argv, "d:l:x:");

  Ofp *ofp = ofp_load("-");

  if (def_file)
    ofp_header(ofp, def_file);
  
  if (list_arg)
    ofp_list(ofp, list_arg);

  FILE *fp = stdout;
  if ('-' != *xml_file)
    if (!(fp = fopen(xml_file, "w")))
      fprintf(stderr, "ofpx: can't write %s: %s\n", xml_file, strerror(errno));
  if (fp)
    {
      ofp_xml(ofp, fp);
      if ('-' != *xml_file)
	fclose(fp);
    }
}

int
opts(int argc, const char *arg)
{
  switch (argc)
    {
    case 'l':
      list_arg = arg;
      break;
    case 'd':
      def_file = arg;
      break;
    case 'x':
      xml_file = arg;
      break;
    }
  return 0;
}

void help(void) { }
