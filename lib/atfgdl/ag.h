#ifndef AG_H_
#define AG_H_

extern const char *curr_line_text;

extern void ag_line_o_mts(Mloc l);
extern void ag_line_o_nts(Mloc l);
extern void ag_line_o_gus(Mloc l);
extern void ag_line_o_var(Mloc l);
extern void ag_line_o_bil(Mloc l);
extern void ag_line_o_lgs(Mloc l);
extern void ag_line_c_mts(Mloc l);
extern void ag_line_c_nts(Mloc l);
extern void ag_line_c_gus(Mloc l);
extern void ag_line_c_var(Mloc l);
extern void ag_line_c_bil(Mloc l);
extern void ag_line_c_lgs(Mloc l);

#endif/*AG_H_*/

