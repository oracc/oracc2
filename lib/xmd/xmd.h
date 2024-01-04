#ifndef _XMD_H
#define _XMD_H
extern void xmd_init(void);
extern void xmd_term(void);
extern Hash *xmd_load(const char *project, const char *pq);
#endif /*_XMD_H*/
