#ifndef GDLR_H_
#define GDLR_H_

#define GDLR_WF_INPUT 	0x0001
#define GDLR_VX_INPUT 	0x0002
#define GDLR_TEXT_ASCII 0x0004
#define GDLR_TEXT_C10E	0x0008
#define GDLR_TEXT_ORIG	0x0010
#define GDLR_VX_CATF	0x0020
#define GDLR_VX_OATF	0x0040
#define GDLR_VX_IDENTITY 0x0080
typedef int (*gdlr_node_fnc)(Node *np, FILE *fp);
typedef gdlr_node_fnc GDLR_config[128];
extern int gdlr_orig_text(Node *c, FILE *fp);
extern int gdlr_c10e_text(Node *c, FILE *fp);
extern int gdlr_ascii_text(Node *c, FILE *fp);
extern gdlr_node_fnc *gr_funcs;
extern gdlr_node_fnc gr_wf_fncs[128];
extern GDLR_config *gdlr_clone_fncs(GDLR_config *f);
extern int gdlr_vx_flags(Node *np, FILE *fp);
extern GDLR_config *vx_oatf_config;
#define gdlr_node gr_funcs[0]
#define gdlr_text gr_funcs[1]
#define gdlr_openers gr_funcs[2]
#define gdlr_closers gr_funcs[3]
#define gdlr_flags gr_funcs[4]

extern void gdl_render_setup_wf(void);
extern GDLR_config *gdl_render_setup_vx(int gdl_vx_mode, gdlr_node_fnc textfunc);
extern GDLR_config gdlr_wfa_config, gdlr_wfc_config, gdlr_wfo_config;
extern void gdlr_ascii_funcs(GDLR_config *cp);
extern int gdlr_vxc_openers(Node *np, FILE *fp);
extern int gdlr_vxc_closers(Node *np, FILE *fp);
extern int gdlr_vx_flags(Node *np, FILE *fp);
extern unsigned char *gdl_render(Node *np, GDLR_config *gc);
extern unsigned char *gdl_render_str(unsigned const char *s, GDLR_config *c);
extern void gdl_output(Node *np, FILE *fp);

extern int gr_node_vx(Node *np, FILE *fp);
extern int gr_node_wf(Node *np, FILE *fp);

#endif/*GDLR_H_*/
