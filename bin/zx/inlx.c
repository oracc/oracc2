#include <oraccsys.h>
#include <xml.h>
#include <scan.h>
#include <inl.h>

int line = 1;

int
main(int argc, char * const *argv)
{
  options(argc, argv, "?");
  mesg_init();
  scan_init();
  inl_init();

  Tree *tp;
  if (argv[optind])
    tp = inl(NULL, strdup(argv[optind]));
  else
    tp = inl(NULL, strdup("a @em{b1 @sub{z} b2} c @br{} @cite[p20]{Englund1999} d @sub[2cm]{e} f"));
  
  tree_ns_default(tp, NS_INL);
  tree_xml(NULL, tp);

  scan_term();
  inl_term();
}

const char *prog = "inlx";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = "";

int
opts(int opt, const char *arg)
{
  switch (opt)
    {
    case '?':
      help();
      exit(1);
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void help (void)
{
  printf("inlx converts a string arg to http://oracc.org/ns/inl/1.0 XML.\n");
}

