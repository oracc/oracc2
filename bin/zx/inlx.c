#include <oraccsys.h>
#include <xml.h>
#include <scan.h>
#include <inl.h>

int
main(int argc, char * const *argv)
{
  scan_init();
  inl_init();
  mesg_init();
  nodeh_register(treexml_o_handlers, NS_INL, treexml_o_generic);
  nodeh_register(treexml_c_handlers, NS_INL, treexml_c_generic);

  Tree *tp = inl(strdup("a"));
  
  tree_ns_default(tp, NS_INL);
  tree_xml(NULL, tp);

  scan_term();
  inl_term();
}
