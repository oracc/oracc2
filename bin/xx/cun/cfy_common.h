#ifndef CFY_COMMON_H
#define CFY_COMMON_H

typedef void (cc_elt)(Cfy*c,Elt*e);

typedef void (cc_l_o)(Cfy*c,Line*l);
typedef void (cc_l_c)(Cfy*c);
typedef void (cc_c_o)(Cfy*c, Elt *e);
typedef void (cc_c_c)(Cfy*c);
typedef void (cc_b_o)(Cfy*c, Btype b);
typedef void (cc_b_c)(Cfy*c, Btype b);

typedef struct tagfuncs
{
  cc_l_o *l_o;
  cc_l_c *l_c;
  cc_b_o *c_o;
  cc_b_c *c_c;
  cc_b_o *b_o;
  cc_b_c *b_c;
} Tagfuncs;

extern Tagfuncs *cc_tags;

extern void cc_body(Cfy *c);

#endif/*CFY_COMMON_H*/
