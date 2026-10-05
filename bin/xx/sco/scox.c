#include <oraccsys.h>
#include <xml.h>

int
main(int argc, char *const*argv)
{
  Tree *tp = xml_load_tree("01bld/linkbase.xml", 0);

  Hash *tags = hash_create(1);
  hash_add(tags, (uccp)"link", "");
  hash_add(tags, (uccp)"refs", "");
  List *tl = xlt_tags_hash(tp->root, tags);
}
