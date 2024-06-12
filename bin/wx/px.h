#ifndef WX_PX_H
#define WX_PX_H

#define PX_EXE "/home/oracc/bin/px"
#define QX_EXE "/home/oracc/bin/qx"

extern void dump_vec(const char *porq, const char **vec);
extern const char *px_cgi_arg(const char *arg, const char *val);
extern void pxglossary(const char *project, const char *elt0);
extern void pxglossary_article(const char *project, const char *glos, const char *gart, const char *form);
extern void pxlist(const char *project, const char *list);

#endif/*WX_PX_H*/
