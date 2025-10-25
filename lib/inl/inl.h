#ifndef INL_H_
#define INL_H_

extern void inl_init(void);
extern void inl_term(void);
extern char *inl_nodes(Node *np, char *s);
extern Tree *inl(char *s);
extern Memo *inl_scan_m;

#endif/*INL_H_*/
