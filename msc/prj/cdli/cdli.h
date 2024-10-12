#ifndef CDLI_H_
#define CDLI_H_

struct periodstab
{
  const char *name;
  const char *orac;
  int sort;
};

extern struct periodstab *periods (register const char *str, register size_t len);

#endif/*CDLI_H_*/
