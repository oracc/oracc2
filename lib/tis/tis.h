#ifndef TIS_H_
#define TIS_H_
#include <hash.h>

typedef Hash *Tisp;

struct tis_data
{
  char *key;
  char *ref;
  char *cnt;
  char *pct;
  char *iss;
  List *subs;
};

extern Tisp tis_init(void);
extern const char **tis_sort(Tisp tp);
extern void tis_add(Tisp tp, const char *key, const char *wid);
extern void tis_dump(FILE *fp, Tisp tp, const char **k);
extern void tis_term(Tisp tp);
extern void tis_load_init(void);
extern void tis_load_term(Tisp tp);
extern Tisp tis_load(FILE *fp);

#endif/*TIS_H_*/
