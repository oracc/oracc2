#include <oraccsys.h>
#include <xml.h>

Hash *P, *PQ, *Q;
Hash *exemplar_lists, *exemplar_links;
Hash *parallel_lists, *source_lists;
Hash *sp;
/*Pool *p;*/

void **
ptrpair(void *vp1, void *vp2)
{
  void **p = malloc(2*sizeof(void *));
  p[0] = vp1;
  p[1] = vp2;
  return p;
}

void
hash_list_add(Hash *h, const char *k, void *vp)
{
  List *lp = hash_find(h, (uccp)k);
  if (!lp)
    hash_add(h, (uccp)k, lp = list_create(LIST_SINGLE));
  list_add(lp, vp);
}

void
create_rel_lists(List *tl)
{
  Node *np;
  for (np = list_first(tl); np; np = list_next(tl))
    {
      if ('l' == *np->name)
	{
	  const char *rel = xlt_att(np, "rel");
	  const char *fref = xlt_att(np->kids, "ref");
	  const char *tref = xlt_att(np->kids->next, "ref");
	  const char *fline = xlt_att(np->kids, "line");
	  const char *tline = xlt_att(np->kids->next, "line");
	  hash_add(PQ, (uccp)fref, "");
	  hash_add(PQ, (uccp)tref, "");
	  hash_add(P, (uccp)fref, "");
	  hash_add(Q, (uccp)tref, "");
	  switch (*rel)
	    {
	    case 'g':
	      hash_list_add(exemplar_lists, tline, (void*)fline);
	      hash_list_add(exemplar_links, fref, ptrpair((void*)fline, (void*)tline));
	      break;
	    case 'p':
	      hash_list_add(parallel_lists, tline, (void*)fline);
	      hash_list_add(parallel_lists, fline, (void*)tline);
	      break;
	    case 'c':
	      hash_list_add(source_lists, fline, (void*)tline);
	      break;
	    default:
	      fprintf(stderr, "scox: corrupt <link> in 01bld/linkbase.xml. Stop.");
	      exit(1);
	    }
	}
      else
	{
	  const char *t = xlt_att(np, "type");
	  switch (*t)
	    {
	    case 's':
	      {
		void **pp = ptrpair(np, NULL);
		hash_add(sp, (uccp)xlt_att(np, "ref"), pp);
	      }
	      break;
	    case 'p':
	      {
		void **pp = ptrpair(NULL, np);
		hash_add(sp, (uccp)xlt_att(np, "ref"), pp);
	      }
	      break;
	    default:
	      fprintf(stderr, "scox: corrupt <ref> in 01bld/linkbase.xml. Stop.");
	      exit(1);
	    }
	}
    }
}

void
scox_init(void)
{
  P = hash_create(128);
  PQ = hash_create(128);
  Q = hash_create(128);
  exemplar_lists = hash_create(128);
  exemplar_links = hash_create(128);
  parallel_lists = hash_create(128);
  source_lists = hash_create(128);
  sp = hash_create(128);
  /*p = pool_init();*/
}

int
main(int argc, char *const*argv)
{
  Tree *tp = xml_load_tree("01bld/linkbase.xml", 0);
  if (!tp)
    {
      fprintf(stderr, "scox: failed to load 01bld/linkbase.xml. Stop.\n");
      exit(1);
    }
  scox_init();
  Hash *tags = hash_create(1);
  hash_add(tags, (uccp)"link", "");
  hash_add(tags, (uccp)"refs", "");
  List *tl = xlt_tags_hash(tp->root, tags);
  create_rel_lists(tl);
}
