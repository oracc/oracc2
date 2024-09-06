#ifndef KEVA_H_
#define KEVA_H_

#include <memo.h>

struct keva
{
  const char *k;
  const char *v;
  void *u; /* user data; oracc/c/otf had 'char *url' */
};

typedef struct keva Keva;

extern Keva *keva_create(Memo *kevamem, const char *key, const char *value);
extern Keva *keva_create3(Memo *kvmem, const char *key, const char *val, void *user);

#endif/*KEVA_H_*/
