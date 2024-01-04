#ifndef _XMD_H
#define _XMD_H
extern void xmd_init(void);
extern void xmd_term(void);
extern Hash *xmd_load(const char *project, const char *pq);
extern const char *xmd_name_c(const char *name);

#endif /*_XMD_H*/
