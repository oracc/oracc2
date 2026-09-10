#include <oraccsys.h>
#include <gdl.h>
#include "vx.h"

Node **tnodes;

static void
vxc_header(Tree *tp, FILE *fp)
{
}

static void
vxc_atf(Node *np, FILE *fp)
{
  if (!strcmp(np->name, "xcl:l"))
    {
      const char *ref = vx_att(np, "ref");
      Node *gdl_np = hash_find(xmlid_h, (uccp)ref);
      vx_atf_node(gdl_np, fp);
    }
}

static void
vxc_cun(Node *np, FILE *fp)
{
  if (!strcmp(np->name, "xcl:l"))
    {
      const char *ref = vx_att(np, "ref");
      Node *gdl_np = hash_find(xmlid_h, (uccp)ref);
      vx_cun_node(gdl_np, fp);
    }
}

static void
vxc_tra(Node *np, FILE *fp)
{
  vx_tra_node(np, fp);
}

static void
vxc_sentences(Tree *tp, FILE *fp)
{
  extern int s_words;
  Node *xcl = vx_component(tp, "xcl:xcl");
  if (xcl)
    {
      List *s = vx_tags_by_attr(xcl, "xcl:c", "type", "sentence", 0);
      if (list_len(s))
	{
	  size_t n_sent = list_len(s);
	  Node **snodes = list2array(s);
	  int i;
	  for (i = 0; snodes[i]; ++i)
	    {
	      Node *sp = snodes[i];
	      char *atf_buf, *cun_buf, *tra_buf;
	      size_t atf_len, cun_len, tra_len;

	      s_words = 0;
	      FILE *atfp = open_memstream(&atf_buf, &atf_len);
	      node_iterator(sp, atfp, (nodehandler)vxc_atf, NULL);
	      fclose(atfp);
	      fprintf(stderr, "%s\n", atf_buf);

	      s_words = 0;
	      FILE *cunp = open_memstream(&cun_buf, &cun_len);
	      node_iterator(sp, cunp, (nodehandler)vxc_cun, NULL);
	      fclose(cunp);
	      fprintf(stderr, "%s\n", cun_buf);

	      s_words = 0;
	      FILE *trap = open_memstream(&tra_buf, &tra_len);
	      node_iterator(tnodes[i], trap, (nodehandler)vxc_tra, NULL);
	      fclose(trap);
	      fprintf(stderr, "%s\n", tra_buf);
	    }
	}
    }
}

Node **
vxc_translations(Tree *tp)
{
   Node *tra = vx_component(tp, "xtr:translation");
   List *divs = vx_tags_by_attr(tra, "xh:div", "data-unit", NULL, 0);
   if (list_len(divs))
     return (Node**)list2array(divs);
   else
     return NULL;
}

void
vx_conllo(Tree *tp, FILE *fp)
{
  vxc_header(tp, fp);
  tnodes = vxc_translations(tp);
  vxc_sentences(tp, fp);
}
