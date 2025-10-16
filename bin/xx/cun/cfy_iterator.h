#ifndef CFY_ITERATOR_H
#define CFY_ITERATOR_H

typedef void (ci_elt)(Cfy*c,Elt*e);

typedef void (ci_l_o)(Cfy*c,Line*l);
typedef void (ci_l_c)(Cfy*c);
typedef void (ci_c_o)(Cfy*c, Cell *e);
typedef void (ci_c_c)(Cfy*c);
typedef void (ci_h_o)(Cfy*c);
typedef void (ci_h_c)(Cfy*c);
typedef void (ci_b_o)(Cfy*c, Btype b);
typedef void (ci_b_c)(Cfy*c);
typedef void (ci_d_o)(Cfy*c); /* div that contains lines, open/close */
typedef void (ci_d_c)(Cfy*c);
typedef void (ci_label)(Cfy*c,const char *label,int print);

typedef struct tagfuncs
{
  ci_l_o *l_o;
  ci_l_c *l_c;
  ci_h_o *h_o; /* h=H(eader), <th> in HTML */
  ci_h_c *h_c;
  ci_c_o *c_o;
  ci_c_c *c_c;
  ci_b_o *b_o;
  ci_b_c *b_c;
  ci_d_o *d_o;
  ci_d_c *d_c;
  ci_label *l;
} Tagfuncs;

extern ci_elt **ci_elt_p;
extern Tagfuncs *ci_tags;

extern void ci_body(Cfy *c);
extern void ci_cell_o(Cfy *c, Elt *e);

#endif/*CFY_ITERATOR_H*/
