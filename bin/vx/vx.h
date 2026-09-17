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

typedef enum omode
  {
    OM_NONE , OM_CATF , OM_CONLLO , OM_IDENTITY
  } Omode;

typedef void (*vx_simples_fnc)(Node *np, FILE *fp);
extern vx_simples_fnc vx_simples_p;

typedef void (*vx_attr_fnc)(Node *np, const char **atts);
extern vx_attr_fnc vx_attr_p;

extern Hash *xtf_a, *xtf_e, *xmlid_h;
extern const char *translation_fn;
extern void vx_attr(Node *np, const char **atts);
extern void vx_attr_xmlid(Node *np, const char **atts);

extern List *vx_tags(Node *np, const char *tag);
extern List *vx_tags_by_attr(Node *np, const char *tag, const char *attr, const char *value, int match);
extern Node *vx_component(Tree *tp, const char *name);
extern Tree *vx_load(const char *fn);
extern const char *vx_att(Node *np, const char *att);
extern void vx_conllo(Tree *tp, FILE *fp);
extern void vx_identity(Tree *tp, FILE *fp);
extern void vx_selector(Node *, Vxsel*);
extern void vx_xtf_init(void);
extern void vx_atf_gdl_node(Node *np, FILE *fp);
extern void vx_atf_xtf_node(Node *np, FILE *fp);
extern void vx_cun_node(Node *np, FILE *fp);
extern void vx_tra_node(Node *np, FILE *fp);
extern void vx_epsd_init(void);
extern const char *vx_epsd_cft(const char *cgp);
extern const char *vx_epsd_dis(const char *cgp);
extern const char *vx_epsd_oid(const char *cgp);
extern const char *vx_epsd_sid(const char *cgspe);
extern const char *vx_epsd_sgw(const char *cgspe);
extern const char *vx_epsd_ucun(const char *tlit);

extern void vxa_openers(Node *np, FILE *fp);
extern void vxa_closers(Node *np, FILE *fp);
extern void vxa_g_delim(Node *np, FILE *fp);
extern void vxa_simples(Node *np, FILE *fp);

extern const char *vxa_prop_val(Node *np, const char *aname);
extern void vxa_status_flags(Node *np, FILE *fp);

typedef struct CATF_helper
{
  FILE *outfp;
  Node *start;
} CATF_helper;

typedef void (vx_catf_fnc)(Node *np, CATF_helper *chp);

typedef struct vxcfnctab
{
  const char *name;
  vx_catf_fnc *fnc;
} Vxcfnctab;

extern struct vxcfnctab *vxcfnctab (register const char *str, size_t len);
extern vx_catf_fnc vxc_composite;
extern vx_catf_fnc vxc_transliteration;
extern vx_catf_fnc vxc_protocols;
extern vx_catf_fnc vxc_protocol;
extern vx_catf_fnc vxc_obj_sur;
extern vx_catf_fnc vxc_column;
extern vx_catf_fnc vxc_l;
extern vx_catf_fnc vxc_xcl;

extern void vx_catf(Tree *tp, FILE *fp);
unsigned char *utf2atf(const unsigned char *src);

#endif/*VX_H_*/
