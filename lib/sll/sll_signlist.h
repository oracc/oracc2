#ifndef SLL_SIGNLIST_H_
#define SLL_SIGNLIST_H_
struct sll_signlist { const char *name; };
extern const char *sll_is_signlist(const char *key);
extern struct sll_signlist *sll_signlist (register const char *str, register size_t len);
#endif/*SLL_SIGNLIST_H_*/
