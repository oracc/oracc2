#ifndef VX_H_
#define VX_H_
#include <hash.h>
#include <tree.h>
#include <xml.h>
#include <runexpat.h>

typedef struct vxud
{
  FILE *fp;
  Tree *tp;
} Vxud;

typedef struct vxsel
{
  const char *tag; /* tag name, may be NULL */
  const char *att; /* att name, may be NULL */
  const char *val; /* att value, may be NULL */
  int match; /* how does val match att content? 0 = exact; 1 = contains */
  List *l; /* list of Node * to add matches to; may not be NULL */
} Vxsel;

extern Hash *xtf_a, *xtf_e;

extern List *vx_tags(Node *np, const char *tag);
extern List *vx_tags_by_attr(Node *np, const char *tag, const char *attr, const char *value, int match);
extern Node *vx_component(Tree *tp, const char *name);
extern Tree *vx_load(const char *fn);
extern const char *vx_att(Node *np, const char *att);
extern void vx_conllo(Tree *tp, FILE *fp);
extern void vx_identity(Tree *tp, FILE *fp);
extern void vx_selector(Node *, Vxsel*);
extern void vx_xtf_init(void);

#endif/*VX_H_*/
