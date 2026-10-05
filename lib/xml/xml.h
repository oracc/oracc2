#ifndef XML_H_
#define XML_H_	1

#include <xmlify.h>

#include <tree.h>
#include <xnn.h>

#ifndef uccp
#define uccp unsigned const char *
#endif

struct xmlhelper {
  FILE *fp;
  void *user;
};

typedef struct nsdata {
  const char *name;
  const char *equiv;
  nscode code;
} NSdata;

typedef struct xltsel
{
  const char *tag; /* tag name, may be NULL */
  const char *att; /* att name, may be NULL */
  const char *val; /* att value, may be NULL */
  int match; /* how does val match att content? 0 = exact; 1 = contains */
  List *l; /* list of Node * to add matches to; may not be NULL */
} XLT_sel;
typedef void (*xlt_attr_fnc)(Node *np, const char **atts);

extern int xml_printing, xml_validating;

typedef struct xmlhelper Xmlhelper;

extern nodehandlers treexml_o_handlers;
extern nodehandlers treexml_p_handlers;
extern nodehandlers treexml_c_handlers;
extern nodehandlers treexml_a_handlers;
extern nodehandlers treexml_u_handlers;

extern int treexml_no_output;

extern void tree_xml_node(Node *np, void *user);
extern void tree_xml_post(Node *np, void *user);

extern void treexml_o_generic(Node *np, void *user);
extern void treexml_c_generic(Node *np, void *user);
extern void tree_ns_xml_print(Tree *tp, FILE *fp);
extern Xmlhelper *xmlh_init(FILE *fp);
extern void xml_attr(const char **atts, FILE *fp);
extern void node_xml(FILE *fp, Node *np);
extern void tree_xml(FILE *fp, Tree *tp);
extern void tree_xml_rnv(FILE *fp, Tree *tp, struct xnn_data *xdp, const char *rncbase);

extern struct nsdata *nsdata (register const char *str, size_t len);
extern void nsdata_set_key_data(NSdata **np);

extern Tree *xml_load_tree(const char *fn, int with_xmlid);
extern List *xlt_tags(Node *np, const char *tag);
extern const char *xlt_att(Node *np, const char *att);
extern List *xlt_tags_by_attr(Node *np, const char *tag, const char *attr, const char *value, int match);

#endif /* XML_H_ */
