#ifndef ATF_BLD_H_
#define ATF_BLD_H_

extern Tree *abt;

extern void abt_add_key_protocol(Mloc *lp, Key *kp);
extern Node *atf_add(const char *s, Mloc *mp);
extern Node *atf_insert(const char *s);
extern Node *atf_node(const char *s, Mloc *mp);
extern Node *atf_push(const char *s, Mloc *mp);

#endif/*ATF_BLD_H_*/
