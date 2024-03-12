#ifndef IKEYSUB_H
#define IKEYSUB_H
struct ikeysubtab
{
  const char *name;
  const char *sub;
};
extern struct ikeysubtab *ikeysub(register const char *str, register size_t len);
#endif/*IKEYSUB_H*/
