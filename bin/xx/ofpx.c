#include <oraccsys.h>
#include "ofp.h"

int tsv_dump = 0;
int xml_dump = 0;

const char *list_arg = NULL;
const char *tsv_file = "-";
const char *xml_file = "-";

int
main(int argc, char **argv)
{
  options(argc, argv, "l:t::x::");

  if (!tsv_dump && !xml_dump)
    tsv_dump = 1;
  
  Ofp *ofp = ofp_load("-");

  if (list_arg)
    ofp_list(ofp, list_arg);

  if (xml_dump)
    {
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

  if (tsv_dump)
    {
      FILE *fp = stdout;
      if ('-' != *tsv_file)
	if (!(fp = fopen(tsv_file, "w")))
	  fprintf(stderr, "ofpx: can't write %s: %s\n", tsv_file, strerror(errno));
      if (fp)
	{
	  ofp_dump(ofp, fp);
	  if ('-' != *tsv_file)
	    fclose(fp);
	}
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
    case 't':
      tsv_dump = 1;
      if (arg)
	tsv_file = arg;
      break;
    case 'x':
      xml_dump = 1;
      if (arg)
	xml_file = arg;
      break;
    }
  return 0;
}

void help(void) { }
