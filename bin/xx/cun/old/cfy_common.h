#ifndef CFY_COMMON_H
#define CFY_COMMON_H

typedef void (cc_elt)(Cfy*c,Elt*e);

typedef void (cc_l_o)(Cfy*c,Line*l);
typedef void (cc_l_c)(Cfy*c);
typedef void (cc_c_o)(Cfy*c, Cell *e);
typedef void (cc_c_c)(Cfy*c);
typedef void (cc_b_o)(Cfy*c, Btype b);
typedef void (cc_b_c)(Cfy*c);

typedef struct tagfuncs
{
  cc_l_o *l_o;
  cc_l_c *l_c;
  cc_c_o *c_o;
  cc_c_c *c_c;
  cc_b_o *b_o;
  cc_b_c *b_c;
} Tagfuncs;

extern cc_elt **cc_elt_p;
extern Tagfuncs *cc_tags;

extern void cc_body(Cfy *c);
extern void cc_cell_o(Cfy *c, Elt *e);

#endif/*CFY_COMMON_H*/
