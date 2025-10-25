#include <oraccsys.h>
#include <xml.h>
#include <scan.h>
#include <inl.h>

int line = 1;

int
main(int argc, char * const *argv)
{
  mesg_init();
  scan_init();
  inl_init();

  Tree *tp = inl(strdup("a @em{b1 @dn{z} b2} c @nl d @up[2cm]{e} f"));
  
  tree_ns_default(tp, NS_INL);
  tree_xml(NULL, tp);

  scan_term();
  inl_term();
}
