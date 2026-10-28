#ifndef AG_H_
#define AG_H_

extern const char *curr_line_text;

extern void ag_wrapup_buffer(void);
extern ssize_t ag_get_lineno(void);
extern Tree *ag_read_atf(char *atf, size_t len);
extern void ag_set_lineno(ssize_t n);
extern void ag_set_file(const char *f);
extern char* ag_set_file_line(char *f, ssize_t *n);
extern void ag_set_tree(Tree *tp);
extern Tree *agyacc(void);

extern void ag_line_o_mts(Mloc l, const char *t);
extern void ag_line_o_nts(Mloc l, const char *t);
extern void ag_line_o_gus(Mloc l, const char *t);
extern void ag_line_o_var(Mloc l, const char *t);
extern void ag_line_o_bil(Mloc l, const char *t);
extern void ag_line_o_lgs(Mloc l, const char *t);
extern void ag_line_c_mts(Mloc l);
extern void ag_line_c_nts(Mloc l);
extern void ag_line_c_gus(Mloc l);
extern void ag_line_c_var(Mloc l);
extern void ag_line_c_bil(Mloc l);
extern void ag_line_c_lgs(Mloc l);

extern int aglineno;

#endif/*AG_H_*/
