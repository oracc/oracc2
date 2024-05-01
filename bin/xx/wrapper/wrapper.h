#ifndef WRAPPER_H_
#define WRAPPER_H_
struct progtab
{
  const char *name;
  const char *sheet;
  const char *sheetpath;
  const char *inext;
  const char *outext;
};

struct progtab *progtab (register const char *str, register size_t len);

#endif/*WRAPPER_H_*/
